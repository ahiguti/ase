
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

// FIXME: aseccloader.cpp

#include <ase/asecpp.hpp>
#include <ase/aseatomic.hpp>
#include <ase/asemutex.hpp>
#include <ase/asestrmap.hpp>
#include <ase/aseweakref.hpp>
#include <ase/asebindmethod.hpp>
#include <ase/aseflags.hpp>
#include <ase/asedllhandle.hpp>
#include <ase/aseio.hpp>
#include <ase/aseproc.hpp>
#include <ase/asestrutil.hpp>
#include <ase/aseintermed.hpp>
#include <ase/aseexcept.hpp>

#include "asenative.hpp"
#include "asesyn.hpp"

#include <memory>
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <map>
#include <iostream>
#include <cctype>
#include <cstdio>

#define DBG_WEAK(x)
#define DBG_CMD(x)

namespace {

struct dll_module {

  dll_module(const std::string& nm, bool short_name,
    const std::vector<std::string>& path);
  ~dll_module(); /* DCHK */

  ase_string name;
  ase_string error;
  ase_string dllpath;
  std::auto_ptr<ase_auto_dll_handle> dllhandle;
  ASE_VariantFactoryPtr service_ptr;

};

dll_module::dll_module(const std::string& nm, bool short_name,
  const std::vector<std::string>& path)
  : name(nm), dllhandle(0), service_ptr(0)
{
  std::vector<std::string>::const_iterator i;
  for (i = path.begin(); i != path.end(); ++i) {
    if (short_name) {
      dllpath = (*i) + ASE_DIR_SEP_STR + name + ASE_DLL_SUFFIX;
    } else {
      dllpath = (*i) + ASE_DIR_SEP_STR "libase-lib-" + name + ASE_DLL_SUFFIX;
    }
    if (ase_file_exists(dllpath)) {
      break;
    }
  }
  if (i == path.end()) {
    error = "module not found: " + name;
    dllpath = ase_string();
    ase_throw_invalid_arg(error.data());
  }
  ASE_VERBOSE_DLL(std::cerr << "DLOPEN " << dllpath << std::endl);
  dllhandle.reset(new ase_auto_dll_handle(dllpath, true));
  if (!dllhandle->GetError().empty()) {
    error = dllhandle->GetError();
    ase_string s = "Native: DLLOpen: " + error;
    ase_throw_io_error(s.data());
  }
  ase_string symname = "ASE_NewVariantFactory_" + name;
  void *sym = dllhandle->get_symbol(symname);
  if (!sym) {
    sym = dllhandle->get_symbol(ase_string("ASE_NewVariantFactory"));
    if (!sym) {
      error = dllhandle->GetError();
      ase_string s = "Native: get_symbol: " + error;
      ase_throw_invalid_arg(s.data());
    }
  }
  typedef ASE_NewVariantFactoryFunc *func_t;
  func_t f = (func_t)sym;
  f(service_ptr);
}

dll_module::~dll_module() /* DCHK */
{
  ASE_VERBOSE_DLL(std::cerr << "DLCLOSE " << dllpath << std::endl);
}

std::string
safe_path(const std::string& str)
{
  std::vector<char> buf;
  for (ase_size_type i = 0; i < str.size(); ++i) {
    char c = str[i];
    if (std::isalnum(c)) {
      buf.push_back(c);
    } else {
      switch (c) {
      case '/':
      case ' ':
      case '_':
      case '-':
      case '.':
      case ':':
	buf.push_back(c);
	break;
      case '\\':
	buf.push_back('/');
	break;
      default:
	break;
      }
    }
  }
  if (buf.empty()) {
    return std::string();
  }
  return std::string(&buf[0], buf.size());
}

struct dll_modules {

  dll_modules(const std::vector<std::string>& pt);
  ~dll_modules(); /* DCHK */
  ase_variant newLibraryObject(const std::string& name, bool short_name,
    ase_unified_engine_ref& ue, const ase_variant& extglobals_weak,
    const std::vector<std::string> *alt_path = 0);
  const std::string& getIncPathSafe() const { return incpathsafe; }
  const std::string& getLibPathSafe() const { return libpathsafe; }

 private:

  typedef std::map<std::string, dll_module *> modmap_type;

  ase_fast_mutex mutex;
  std::vector<std::string> modules_path;
  modmap_type modmap;
  std::string incpathsafe, libpathsafe;

 private:

  /* noncopyable */
  dll_modules(const dll_modules& x);
  dll_modules& operator =(const dll_modules& x);

};

dll_modules::dll_modules(const std::vector<std::string>& pt)
  : modules_path(pt)
{
  ase_size_type i;
  for (i = 0; i < pt.size(); ++i) {
    std::string p = pt[i] + ASE_DIR_SEP_STR + "ase" + ASE_DIR_SEP_STR
      + "asecpp.hpp";
    if (ase_file_exists(p)) {
      #ifdef __GNUC__
      incpathsafe = "-I\"" + safe_path(pt[i]) + "\"";
      #else
      incpathsafe = "/I\"" + safe_path(pt[i]) + "\"";
      libpathsafe = "\"" + safe_path(pt[i]) + "/aselib.lib\"";
      #endif
    }
  }
}

dll_modules::~dll_modules() /* DCHK */
{
  modmap_type::const_iterator iter;
  for (iter = modmap.begin(); iter != modmap.end(); ++iter) {
    delete iter->second;
  }
}

ase_variant
dll_modules::newLibraryObject(const std::string& name, bool short_name,
  ase_unified_engine_ref& ue, const ase_variant& extglobals_weak,
  const std::vector<std::string> *alt_path)
{
  ase_mutex_guard<ase_fast_mutex> g(mutex);
  ASE_VariantFactory *service_ptr = 0;
  modmap_type::const_iterator iter = modmap.find(name);
  if (iter != modmap.end()) {
    service_ptr = iter->second->service_ptr.get();
  } else {
    std::auto_ptr<dll_module> p(new dll_module(name, short_name,
      alt_path ? (*alt_path) : modules_path));
    modmap[name] = p.get();
    service_ptr = p->service_ptr.get();
    p.release();
  }
  ase_variant r = service_ptr->NewVariant(ue, extglobals_weak);
  return r;
}

struct ase_native_variant_impl : public ase_variant_impl_default {

  ase_native_variant_impl(ase_unified_engine_ref& ue,
    dll_modules *mod, const ase_variant& extgls_weak,
      const ase_variant& syn_strong)
    : refcount(1), ueref(ue), modules(mod), extglobals_weak(extgls_weak),
      syntax_klass_strong(syn_strong) {
  }

  static ase_native_variant_impl *get(const ase_variant& self) {
    return static_cast<ase_native_variant_impl *>(self.get_rep().p);
  }

  static void add_ref(const ase_variant& self) {
    get(self)->refcount++;
  }
  static void release(const ase_variant& self) {
    ase_native_variant_impl *p = get(self);
    if ((p->refcount--) == 1) {
      ase_variant::debug_on_destroy_object(self, "Native");
      delete p;
    }
  }
  static ase_variant get_property(const ase_variant& self,
    const char *name, ase_size_type namelen);
  static ase_variant invoke_by_name(const ase_variant& self,
    const char *name, ase_size_type namelen, const ase_variant *args,
    ase_size_type nargs);
  static ase_variant GetWeak(const ase_variant& self);

  void compile_file(const std::string& fname, const std::string& sofname);
  ase_variant load_file(const ase_string& fname);

 private:

  typedef std::map<std::string, ase_variant> libmap_type;

  ase_atomic_count refcount;
  ase_unified_engine_ref& ueref;
  dll_modules *modules;
  ase_variant extglobals_weak;
  libmap_type libmap;
  ase_variant syntax_klass_strong;

};

struct ase_native_weak_variant_impl : public ase_native_variant_impl {

  static void add_ref(const ase_variant& self) { /* WEAK */ }
  static void release(const ase_variant& self) { /* WEAK */ }

};

}; // anonymous namespace

ase_variant
ase_native_variant_impl::get_property(const ase_variant& self,
  const char *name, ase_size_type namelen)
{
  if (ase_strfind(name, namelen, '.')) {
    return ase_get_property_chained(self, name, namelen, '.');
  }
  ase_native_variant_impl *p = get(self);
  std::string n(name, namelen);
  if (n == "ExecuteFile") {
    return ase_new_variant_bind_method_name(self, ase_string(n));
  }
  if (n == "Syntax") {
    return p->syntax_klass_strong.GetWeak();
  }
  libmap_type::const_iterator iter = p->libmap.find(n);
  if (iter != p->libmap.end()) {
    return iter->second.GetWeak();
  }
  ase_variant v = p->modules->newLibraryObject(n, false, p->ueref,
    p->extglobals_weak);
  p->libmap[n] = v;
  return v.GetWeak();
}

#ifdef __GNUC__
struct auto_pipe {
  auto_pipe(FILE *p) : fp(p) { }
  int close() {
    if (fp == 0) {
      return 0;
    }
    int r = pclose(fp);
    fp = 0;
    return r;
  }
  ~auto_pipe() /* DCHK */ { close(); }
  FILE *get() const { return fp; }
 private:
  FILE *fp;
  auto_pipe(const auto_pipe& x);
  auto_pipe& operator =(const auto_pipe& x);
};
#endif

#define STRINGIZE(x) STRINGIZE_(x)
#define STRINGIZE_(x) #x

void
ase_native_variant_impl::compile_file(const std::string& fname,
  const std::string& sofname)
{
  if (!ase_file_exists(fname) && ase_file_exists(sofname)) {
    return;
  }
  if (ase_newer_than(sofname, fname)) {
    return;
  }
  #ifdef __GNUC__
  /* TODO: use execve instead */
  std::string cmd;
  #ifdef ASE_CXX
  cmd = STRINGIZE(ASE_CXX);
  #else
  cmd = "g++";
  #endif
  cmd += " -O3 -fPIC -Werror -Wall -g -shared "
    + modules->getIncPathSafe() + " \"" + safe_path(fname)
    + "\" -o \"" + safe_path(sofname) + "\" 2>&1";
  DBG_CMD(fprintf(stderr, "CMD: %s\n", cmd.c_str()));
  auto_pipe pi(popen(cmd.c_str(), "r"));
  std::vector<char> rbuf;
  while (true) {
    char buf[256];
    size_t l = fread(buf, 1, 256, pi.get());
    rbuf.insert(rbuf.end(), buf, buf + l);
    if (l == 0) {
      break;
    }
  }
  if (pi.close() != 0) {
    ase_string s;
    if (rbuf.empty()) {
      s = "compilation failed: command= " + ase_string(cmd.data(), cmd.size());
    } else {
      s = "compilation failed: command= " + ase_string(cmd.data(), cmd.size())
	+ "\n" + ase_string(&rbuf[0], rbuf.size());
    }
    ase_throw_error(s.data());
  }
  #else
  std::string cmd = "cl /EHsc /FD /MD " + modules->getIncPathSafe() +
    " \"" + safe_path(fname) + "\" " + modules->getLibPathSafe() +
    " /link /DLL /OUT:\"" + safe_path(sofname) + "\"";
  ase_string out;
  int rc = ase_process_read(ase_string("cl"), ase_string(cmd.data(), cmd.size()),
    out);
  if (rc != 0) {
    ase_string s;
    if (out.size() == 0) {
      s = "compilation failed: command= " + ase_string(cmd.data(), cmd.size());
    } else {
      s = "compilation failed: command= " + ase_string(cmd.data(), cmd.size())
	+ "\n" + out;
    }
    ase_throw_error(s.data());
  }
  #endif
}

ase_variant
ase_native_variant_impl::load_file(const ase_string& filename)
{
  typedef std::string::size_type size_type;
  std::string fname(filename.data(), filename.size());
  size_type dot = fname.rfind('.');
  if (dot != fname.npos && fname.substr(dot) == ".cpp") {
    std::string sofname = fname.substr(0, dot) + ASE_DLL_SUFFIX;
    this->compile_file(fname, sofname);
    fname = sofname;
  }
  std::string dname;
  size_type slash = fname.rfind('/');
  if (slash != fname.npos) {
    dname = fname.substr(0, slash);
    fname = fname.substr(slash + 1);
  }
  bool short_name = false;
  std::string libname_pre = "libase-lib-";
  std::string libname_suf = ASE_DLL_SUFFIX;
  std::string mname;
  if (fname.size() > libname_pre.size() + libname_suf.size()) {
    size_type mnlen = fname.size()
      - libname_pre.size() - libname_suf.size();
    mname = fname.substr(libname_pre.size(), mnlen);
  }
  if (libname_pre + mname + libname_suf != fname) {
    mname = "";
  }
  if (mname.empty()) {
    if (fname.size() > libname_suf.size() &&
      fname.substr(fname.size() - libname_suf.size()) == libname_suf) {
      short_name = true;
      mname = fname.substr(0, fname.size() - libname_suf.size());
    }
  }
  ASE_VERBOSE_DLL(std::cerr << "mname=" << mname << " " << short_name
    << std::endl);
  const std::string& n = mname;
  libmap_type::const_iterator iter = this->libmap.find(n);
  if (iter != this->libmap.end()) {
    ASE_VERBOSE_DLL(std::cerr << "mname=" << mname << " already loaded"
      << std::endl);
    return iter->second.GetWeak();
  }
  std::vector<std::string> pt;
  pt.push_back(dname);
  ase_variant v = this->modules->newLibraryObject(n, short_name, this->ueref,
    this->extglobals_weak, dname.empty() ? 0 : (&pt));
  this->libmap[n] = v; /* we must keep klass-object valid until the engine
    is destroyed. */
  return v.GetWeak();
}

ase_variant
ase_native_variant_impl::invoke_by_name(const ase_variant& self,
  const char *name, ase_size_type namelen, const ase_variant *args,
  ase_size_type nargs)
{
  /* this function need not to be so fast */
  ase_native_variant_impl *p = get(self);
  if (ase_string::equals(name, namelen, "ExecuteFile")) {
    /* execute a dll file */
    if (nargs < 1) {
      ase_throw_missing_arg("Native: Usage: Native.ExecuteFile(\"FILENAME.so\")");
    }
    ase_variant rfunc = p->load_file(args[0].get_string());
    return rfunc.invoke_self(args + 1, nargs - 1);
  }
  return ase_variant();
}

ase_variant
ase_native_variant_impl::GetWeak(const ase_variant& self)
{
  DBG_WEAK(fprintf(stderr, "nvi getweak\n"));
  ase_variant r = ase_variant::create_object(
    &ase_variant_impl<ase_native_weak_variant_impl>::vtable,
    get(self),
    0 /* weak */);
  return r;
}

namespace {

struct ase_native_script_engine : public ase_script_engine {

  ase_native_script_engine(ase_unified_engine_ref& ue, dll_modules *mod,
    ASE_VariantFactoryPtr& syntax_klassfac)
    : ueref(ue) {
    ase_variant synklass_strong = ASE_NewStrongReference(
      syntax_klassfac->NewVariant(ue, ase_variant()));
    extglobals = ase_new_string_map();
    extglobals_strong = ASE_NewStrongReference(
      ase_string_map_unmodifiable(extglobals));
    extglobals_weak = extglobals_strong.GetWeak();
    nativelibs = ase_variant::create_object(
      &ase_variant_impl<ase_native_variant_impl>::vtable,
      new ase_native_variant_impl(ueref, mod, extglobals_weak,
	synklass_strong),
      "Native");
    DBG_WEAK(fprintf(stderr, "nse getweak\n"));
    nativelibs_weak = nativelibs.GetWeak();
    extglobals.set("Native", nativelibs_weak);
  }
  void destroy() {
    delete this;
  }
  void child_thread_initialized() {
  }
  void child_thread_terminated() {
  }
  void ForceGC() {
  }
  void WeakenProxies() {
  }
  void wait_threads() {
  }
  void dispose_proxies() {
    extglobals = ase_variant();
    extglobals_strong = ase_variant();
    extglobals_weak = ase_variant();
  }
  const char *const *get_global_names() {
    static const char *const globals[] = { "Native", 0 };
    return globals;
  }
  ase_variant get_global_object(const char *name) {
    std::string s(name);
    if (s == "Native") {
      return nativelibs_weak;
    }
    return ase_variant();
  }
  void import_global(const char *name, const ase_variant& val) {
    extglobals.set_property(name, std::strlen(name), val);
  }
  ase_variant LoadFile(const char *glname, const char *filename) {
    ase_native_variant_impl *p = ase_native_variant_impl::get(nativelibs);
    return p->load_file(ase_string(filename));
  }

 private:

  ase_unified_engine_ref& ueref;
  ase_environment *shared_data;
  ase_variant extglobals;
  ase_variant extglobals_strong;
  ase_variant extglobals_weak;
  ase_variant nativelibs;
  ase_variant nativelibs_weak;

};

struct ase_native_engine_factory : public ase_script_engine_factory {

  ase_native_engine_factory(ase_environment& env) {
    const char *const *path = env.get_path();
    // ASE_NewVariantFactory_Syntax(syntax_klassfac);
    std::vector<std::string> pt;
    for (; path != 0 && (*path) != 0; ++path) {
      pt.push_back(std::string(*path));
    }
    modules.reset(new dll_modules(pt));
  }
  ~ase_native_engine_factory() /* DCHK */ {
  }
  void destroy() {
    delete this;
  }
  void new_script_engine(ase_unified_engine_ref& ue, ase_script_engine_ptr& ap_r) {
    ap_r.reset(new ase_native_script_engine(ue, modules.get(),
      syntax_klassfac));
  }

 private:

  std::auto_ptr<dll_modules> modules;
  ASE_VariantFactoryPtr syntax_klassfac;

};

}; // anonymous namespace

extern "C" void
ASE_NewScriptEngineFactory_Native(ase_environment& env,
  ase_script_engine_factory_ptr& ap_r)
{
  ap_r.reset(new ase_native_engine_factory(env));
}

