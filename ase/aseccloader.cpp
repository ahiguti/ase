
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/ase.hpp>
#include <ase/aseatomic.hpp>
#include <ase/asemutex.hpp>
#include <ase/asestrmap.hpp>
#include <ase/asebindmethod.hpp>
#include <ase/aseflags.hpp>
#include <ase/asedllhandle.hpp>
#include <ase/aseio.hpp>
#include <ase/aseproc.hpp>
#include <ase/asestrutil.hpp>
#include <ase/aseintermed.hpp>
#include <ase/aseexcept.hpp>

#include "aseccloader.hpp"

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
#define DBG_REF(x)

namespace {

struct dll_module {

  dll_module(const std::string& nm, bool short_name,
    const std::vector<std::string>& path, const ase_variant& extmodules_unmod);
  ~dll_module(); /* DCHK */

  ase_string name;
  ase_string error;
  ase_string dllpath;
  std::auto_ptr<ase_auto_dll_handle> dllhandle;
  ase_variant klass;

};

struct dll_modules {

  dll_modules(const std::vector<std::string>& pt);
  ~dll_modules(); /* DCHK */
  ase_variant new_library_object(const std::string& name, bool short_name,
    ase_unified_engine_ref& ue, const std::vector<std::string> *alt_path,
    const ase_variant& ase_global);
  const std::string& get_inc_path_safe() const { return incpathsafe; }
  const std::string& get_lib_path_safe() const { return libpathsafe; }

 private:

  typedef std::map<std::string, dll_module *> modmap_type;

  ase_recursive_mutex mutex;
  std::vector<std::string> modules_path;
  modmap_type modmap;
  std::string incpathsafe, libpathsafe;

 private:

  /* noncopyable */
  dll_modules(const dll_modules& x);
  dll_modules& operator =(const dll_modules& x);

};

struct ase_ccldr_script_engine;

struct ase_ccldr_variant_impl : public ase_variant_impl_default {

  ase_ccldr_variant_impl(ase_unified_engine_ref& ue,
    dll_modules *mod, ase_ccldr_script_engine& se_bref)
    : refcount(1), ueref(ue), modules(mod), se_backref(se_bref) {
  }

  static ase_ccldr_variant_impl *get(const ase_variant& self) {
    return static_cast<ase_ccldr_variant_impl *>(self.get_rep().p);
  }

  static void add_ref(const ase_variant& self) {
    get(self)->refcount++;
  }
  static void release(const ase_variant& self) {
    ase_ccldr_variant_impl *p = get(self);
    if ((p->refcount--) == 1) {
      ase_variant::debug_on_destroy_object(self, "CPP");
      delete p;
    }
  }
  static ase_variant get_property(const ase_variant& self,
    const char *name, ase_size_type namelen);
  static ase_variant invoke_by_name(const ase_variant& self,
    const char *name, ase_size_type namelen, const ase_variant *args,
    ase_size_type nargs);

  void compile_file(const std::string& fname, const std::string& sofname);
  ase_variant load_file(const ase_string& fname);

 private:

  typedef std::map<std::string, ase_variant> libmap_type;

  ase_atomic_count refcount;
  ase_unified_engine_ref& ueref;
  dll_modules *modules;
  ase_ccldr_script_engine& se_backref;
  libmap_type libmap;

};

struct ase_ccldr_script_engine : public ase_script_engine {

  ase_ccldr_script_engine(ase_unified_engine_ref& ue, dll_modules *mod);
  void destroy();
  void child_thread_initialized();
  void child_thread_terminated();
  void force_gc();
  void wait_threads();
  void dispose_proxies();
  const char *const *get_global_names();
  ase_variant get_global_object(const char *name);
  void import_ase_global(const ase_variant& val);
  ase_variant load_file(const char *glname, const char *filename);
  ase_script_engine *get_real_engine();

  ase_variant get_ase_global() const { return ase_global; }

 private:

  ase_unified_engine_ref& ueref;
  ase_environment *shared_data;
  ase_variant ase_global;
  ase_variant cclibs;

};


dll_module::dll_module(const std::string& nm, bool short_name,
  const std::vector<std::string>& path, const ase_variant& ase_global)
  : name(nm), dllhandle(0)
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
  if (!dllhandle->get_error().empty()) {
    error = dllhandle->get_error();
    ase_string s = "CPP: DLLOpen: " + error;
    ase_throw_io_error(s.data());
  }
  ase_string symname = "ASE_DLLMain_" + name;
  void *sym = dllhandle->get_symbol(symname);
  if (!sym) {
    sym = dllhandle->get_symbol(ase_string("ASE_DLLMain"));
    if (!sym) {
      error = dllhandle->get_error();
      ase_string s = "CPP: get_symbol: " + error;
      ase_throw_invalid_arg(s.data());
    }
  }
  typedef ase_variant (*func_t)(const ase_variant&);
  func_t f = (func_t)sym;
  klass = (*f)(ase_global);
  ASE_VERBOSE_DLL(std::cerr << "KLASS " << klass << std::endl);
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

dll_modules::dll_modules(const std::vector<std::string>& pt)
  : modules_path(pt)
{
  ase_size_type i;
  for (i = 0; i < pt.size(); ++i) {
    std::string p = pt[i] + ASE_DIR_SEP_STR + "ase" + ASE_DIR_SEP_STR
      + "ase.hpp";
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
dll_modules::new_library_object(const std::string& name, bool short_name,
  ase_unified_engine_ref& ue, const std::vector<std::string> *alt_path,
  const ase_variant& ase_global)
{
  ase_mutex_guard<ase_recursive_mutex> g(mutex);
  ase_variant klass;
  modmap_type::const_iterator iter = modmap.find(name);
  if (iter != modmap.end()) {
    klass = iter->second->klass;
  } else {
    std::auto_ptr<dll_module> p(new dll_module(name, short_name,
      (alt_path ? (*alt_path) : modules_path), ase_global));
    dll_module *& v = modmap[name];
    delete v;
      /* can be non-null when the same module is loaded recursively? */
    v = p.get();
    klass = p->klass;
    p.release();
  }
  return klass;
}

ase_variant
ase_ccldr_variant_impl::get_property(const ase_variant& self,
  const char *name, ase_size_type namelen)
{
  if (ase_strfind(name, namelen, '.')) {
    return ase_get_property_chained(self, name, namelen, '.');
  }
  ase_ccldr_variant_impl *p = get(self);
  std::string n(name, namelen);
  if (n == "ExecuteFile") {
    return ase_new_variant_bind_method_name(self, ase_string(n));
  }
  libmap_type::const_iterator iter = p->libmap.find(n);
  if (iter != p->libmap.end()) {
    return iter->second;
  }
  ase_variant v = p->modules->new_library_object(n, false, p->ueref, 0,
    p->se_backref.get_ase_global());
  p->libmap[n] = v;
  return v;
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
ase_ccldr_variant_impl::compile_file(const std::string& fname,
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
    + modules->get_inc_path_safe() + " \"" + safe_path(fname)
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
  std::string cmd = "cl /EHsc /FD /MD " + modules->get_inc_path_safe() +
    " \"" + safe_path(fname) + "\" " + modules->get_lib_path_safe() +
    " /link /DLL /OUT:\"" + safe_path(sofname) + "\"";
  ase_string out;
  int rc = ase_process_read(ase_string("cl"),
    ase_string(cmd.data(), cmd.size()), out);
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
ase_ccldr_variant_impl::load_file(const ase_string& filename)
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
  ASE_VERBOSE_DLL(std::cerr << "mname=[" << mname << "] [" << short_name
    << "]" << std::endl);
  const std::string& n = mname;
  libmap_type::const_iterator iter = this->libmap.find(n);
  if (iter != this->libmap.end()) {
    ASE_VERBOSE_DLL(std::cerr << "mname=" << mname << " already loaded"
      << std::endl);
    return iter->second;
  }
  std::vector<std::string> pt;
  pt.push_back(dname);
  ase_variant v = this->modules->new_library_object(n, short_name, this->ueref,
    (dname.empty() ? 0 : (&pt)), this->se_backref.get_ase_global());
  this->libmap[n] = v; /* we must keep klass-object valid until the engine
    is destroyed. */
  ASE_VERBOSE_DLL(std::cerr << "mname=[" << mname << "] lib=[" << v
    << "]" << std::endl);
  return v;
}

ase_variant
ase_ccldr_variant_impl::invoke_by_name(const ase_variant& self,
  const char *name, ase_size_type namelen, const ase_variant *args,
  ase_size_type nargs)
{
  /* this function need not to be so fast */
  ase_ccldr_variant_impl *p = get(self);
  if (ase_string::equals(name, namelen, "ExecuteFile")) {
    /* execute a dll file */
    if (nargs < 1) {
      ase_throw_missing_arg("CPP: Usage: CPP.ExecuteFile(\"FILENAME.so\")");
    }
    return p->load_file(args[0].get_string());
  }
  return ase_variant();
}

ase_ccldr_script_engine::ase_ccldr_script_engine(ase_unified_engine_ref& ue,
  dll_modules *mod)
  : ueref(ue)
{
  cclibs = ase_variant::create_object(
    &ase_variant_impl<ase_ccldr_variant_impl>::vtable,
    new ase_ccldr_variant_impl(ueref, mod, *this),
    "CPP");
}

void
ase_ccldr_script_engine::destroy()
{
  delete this;
}

void
ase_ccldr_script_engine::child_thread_initialized()
{
}

void
ase_ccldr_script_engine::child_thread_terminated()
{
}

void
ase_ccldr_script_engine::force_gc()
{
}

void
ase_ccldr_script_engine::wait_threads()
{
  DBG_REF(printf("CPP:wait_threads\n"));
}

void
ase_ccldr_script_engine::dispose_proxies()
{
  DBG_REF(printf("CPP:dispose_proxies\n"));
  ase_global = ase_variant();
  cclibs = ase_variant();
}

const char *const *
ase_ccldr_script_engine::get_global_names()
{
  static const char *const globals[] = { "CPP", 0 };
  return globals;
}

ase_variant
ase_ccldr_script_engine::get_global_object(const char *name)
{
  std::string s(name);
  if (s == "CPP") {
    return cclibs;
  }
  return ase_variant();
}

void
ase_ccldr_script_engine::import_ase_global(const ase_variant& val)
{
  ase_global = val;
}

ase_variant
ase_ccldr_script_engine::load_file(const char *glname, const char *filename)
{
  ase_ccldr_variant_impl *p = ase_ccldr_variant_impl::get(cclibs);
  return p->load_file(ase_string(filename));
}

ase_script_engine *
ase_ccldr_script_engine::get_real_engine()
{
  return this;
}

struct ase_ccldr_engine_factory : public ase_script_engine_factory {

  ase_ccldr_engine_factory(ase_environment& env) {
    const char *const *path = env.get_path();
    std::vector<std::string> pt;
    for (; path != 0 && (*path) != 0; ++path) {
      pt.push_back(std::string(*path));
    }
    modules.reset(new dll_modules(pt));
  }
  ~ase_ccldr_engine_factory() /* DCHK */ {
  }
  void destroy() {
    delete this;
  }
  void new_script_engine(ase_unified_engine_ref& ue, ase_script_engine_ptr& ap_r) {
    ap_r.reset(new ase_ccldr_script_engine(ue, modules.get()));
  }

 private:

  std::auto_ptr<dll_modules> modules;

};

}; // anonymous namespace

extern "C" void
ASE_NewScriptEngineFactory_CPP(ase_environment& env,
  ase_script_engine_factory_ptr& ap_r)
{
  ap_r.reset(new ase_ccldr_engine_factory(env));
}

