
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/aseruntime.hpp>
#include <ase/asedelayscr.hpp>
#include <ase/asedllhandle.hpp>
#include <ase/asemutex.hpp>
#include <ase/aseatomic.hpp>
#include <ase/aseflags.hpp>
#include <ase/aseio.hpp>
#include <ase/asestrutil.hpp>
#include <ase/aseexcept.hpp>
#include <ase/asealgo.hpp>
#include <ase/asestrmap.hpp>
#include <ase/asearray.hpp>
#include <ase/asefuncobj.hpp>

#include <memory>
#include <set>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <fstream>

#ifndef ASE_DEFAULT_LIBEXEC
#ifdef __GNUC__
#define ASE_DEFAULT_LIBEXEC "/usr/libexec/ase"
#else
#define ASE_DEFAULT_LIBEXEC ""
#endif
#endif

#define DBG_GLOBAL(x)
#define DBG_UNKNOWN(x)
#define DBG_HOST(x)
#define DBG_EXIT(x)

namespace {

struct ase_delay_engine_factory;
struct ase_delay_script_engine;

struct handler_type {
  ase_string globalname;
  handler_type() { }
  handler_type(const ase_string& g)
    : globalname(g) { }
};

struct global_props {
  ase_string globalname;
  std::vector<ase_string> filetypes;
};

struct engine_props {
  ase_string servicename;
  ase_string dllfilename;
  std::vector<global_props> globals;
};

typedef std::map<ase_string, engine_props> confmap_type;
typedef std::map<ase_string, handler_type> handler_map_type;

ase_string
get_dll_filename(const ase_string& servicename)
{
  return ase_string("libase-scr-") + servicename + ASE_DLL_SUFFIX;
}

void
ase_chdir(const ase_string& dname)
{
  if (!ase_change_directory(dname)) {
    ase_string s = "ASE: failed to change directory: " + dname;
    ase_throw_io_error(s.data());
  }
}

void
ase_chdir_to_argv0(const ase_string& argv0)
{
  const char *sl = ase_strrfind(argv0, ASE_DIR_SEP_CHAR);
  if (sl) {
    ase_string dp(argv0.data(), sl - argv0.data());
    ase_chdir(dp);
  }
}

bool
get_token(std::string& buf, std::string& tok_r)
{
  if (buf.empty()) {
    return false;
  }
  std::string::size_type pos = buf.find('\t');
  if (pos == buf.npos) {
    tok_r = buf;
    buf.clear();
  } else {
    tok_r = buf.substr(0, pos);
    buf.erase(0, pos + 1);
  }
  return true;
}

bool
parse_line(std::string& line, std::string& glname_r,
  std::string& svcname_r, std::string& suffix_r)
{
  std::string uestr;
  if (!get_token(line, glname_r) ||
      !get_token(line, svcname_r) ||
      !get_token(line, suffix_r)) {
    return false;
  }
  if (suffix_r == "-") {
    suffix_r = "";
  }
  return true;
}

typedef std::vector<ase_string> dllpath_type;
typedef std::vector<const char *> rdllpath_type;
typedef std::vector<ase_script_engine_factory *> servarr_type;

struct array_create {
  ase_variant operator ()(const ase_variant *args, ase_size_type nargs) {
    return ase_new_array(args, nargs);
  }
};

struct strmap_create {
  ase_variant operator ()(const ase_variant *args, ase_size_type nargs) {
    return ase_new_string_map();
  }
};

struct load_file_funcobj {
  load_file_funcobj(ase_unified_engine *ue) : uengine(ue) { }
  ase_variant operator ()(const ase_variant *args, ase_size_type nargs) {
    if (nargs < 1) {
      return ase_variant();
    }
    return uengine->load_file(args[0].get_string());
  }
  ase_unified_engine *const uengine;
};

}; // anonymous namespace



namespace {

ase_atomic_count ase_variant_engine_count_create = 0;
ase_atomic_count ase_variant_engine_count_destroy = 0;
ase_atomic_count ase_variant_efac_count_create = 0;
ase_atomic_count ase_variant_efac_count_destroy = 0;
ase_atomic_count ase_variant_eshared_count_create = 0;
ase_atomic_count ase_variant_eshared_count_destroy = 0;
ase_atomic_count ase_variant_uengine_count_create = 0;
ase_atomic_count ase_variant_uengine_count_destroy = 0;

};

ase_script_engine::ase_script_engine()
{
  ASE_DEBUG_VARIANT_COUNT_DO(ase_variant_engine_count_create++);
}

ase_script_engine::~ase_script_engine() /* DCHK */
{
  ASE_DEBUG_VARIANT_COUNT_DO(ase_variant_engine_count_destroy++);
}

ase_script_engine_factory::ase_script_engine_factory()
{
  ASE_DEBUG_VARIANT_COUNT_DO(ase_variant_efac_count_create++);
}

ase_script_engine_factory::~ase_script_engine_factory() /* DCHK */
{
  ASE_DEBUG_VARIANT_COUNT_DO(ase_variant_efac_count_destroy++);
}

ase_environment::ase_environment()
{
  ASE_DEBUG_VARIANT_COUNT_DO(ase_variant_eshared_count_create++);
}

ase_environment::~ase_environment() /* DCHK */
{
  ASE_DEBUG_VARIANT_COUNT_DO(ase_variant_eshared_count_destroy++);
}

ase_unified_engine_ref::ase_unified_engine_ref()
{
  ASE_DEBUG_VARIANT_COUNT_DO(ase_variant_uengine_count_create++);
}

ase_unified_engine_ref::~ase_unified_engine_ref() /* DCHK */
{
  ASE_DEBUG_VARIANT_COUNT_DO(ase_variant_uengine_count_destroy++);
}

struct ase_unified_engine::impl_type : public ase_noncopyable {

  struct global_info {
    ase_script_engine *engine;
    ase_variant global;
    global_info(ase_script_engine *e = 0, const ase_variant& g = ase_variant())
      : engine(e), global(g) { }
  };

  typedef std::vector<ase_script_engine *> engines_type;
  typedef std::map<ase_string, global_info> globals_type;

  ase_runtime& conf;
  engines_type engines;
  globals_type globals;
  ase_variant aseglobal, aseglobal_unmod;
  ase_recursive_mutex mutex;

  global_info get_global(const ase_string& name) const;
  void prepare_globals();

  impl_type(ase_runtime& cnf);
  ~impl_type(); /* DCHK */

};

struct ase_runtime::impl_type {

  servarr_type servarr;
  confmap_type confmap;
  handler_map_type handlermap;
  dllpath_type dllpath;
  rdllpath_type rdllpath;
  std::vector<ase_variant> default_argv;
  typedef std::map<std::string, void *> preexisting_engines_type;
  preexisting_engines_type preexisting_engines;

  impl_type() { }
  ~impl_type() /* DCHK */ {
    for (ase_size_type i = 0; i < servarr.size(); ++i) {
      DBG_EXIT(fprintf(stderr, "destr %d begin\n", (int)i));
      servarr[i]->destroy();
      DBG_EXIT(fprintf(stderr, "destr %d begin\n", (int)i));
    }
  }
  bool started() const;
  void start(ase_runtime& cnf);
  ase_string find_dllfile(const ase_string& servicename) const;
  void init_args(const ase_string& argv0, int argc, char **argv);
  void set_preexisting_engines(const ase_preexisting_engine_info *emb,
    size_t num_emb);
  void add_dllpath(const ase_string& dname);
  void register_global(const char *globalname, const char *servicename,
    const char *typestr);
  void register_globals();
  ase_string find_handler(const ase_string& ftype) const;

};

ase_unified_engine::impl_type::impl_type(ase_runtime& cnf)
  : conf(cnf)
{
  aseglobal = ase_new_string_map();
  aseglobal_unmod = ase_string_map_unmodifiable(aseglobal);
}

ase_unified_engine::impl_type::~impl_type() /* DCHK */
{
  globals.clear();
  ase_string_map_clear(aseglobal);
  for (ase_size_type i = 0; i < engines.size(); ++i) {
    try {
      engines[i]->dispose_proxies();
    } catch (...) {
    }
  }
  for (ase_size_type i = 0; i < engines.size(); ++i) {
    engines[i]->destroy();
  }
}

void
ase_unified_engine::impl_type::prepare_globals()
{
  const ase_size_type n = engines.size();
  engines_type& eng = engines;
  for (ase_size_type i = 0; i < n; ++i) {
    ase_script_engine& sc = *eng[i];
    const char *const *const glnames = sc.get_global_names();
    for (const char *const *gln = glnames; gln && (*gln); ++gln) {
      const ase_variant glo = sc.get_global_object(*gln);
      const ase_string glnstr(*gln);
      globals[glnstr] = global_info(&sc, glo);
      aseglobal.set_property(glnstr, glo);
    }
  }
  for (ase_size_type i = 0; i < n; ++i) {
    ase_script_engine& sc = *eng[i];
    sc.import_ase_global(aseglobal_unmod);
  }
}

ase_unified_engine::impl_type::global_info
ase_unified_engine::impl_type::get_global(const ase_string& name) const
{
  globals_type::const_iterator i = globals.find(name);
  if (i == globals.end()) {
    return global_info(0, ase_variant());
  }
  return i->second;
}

ase_unified_engine::ase_unified_engine(ase_runtime& cnf)
  : impl(0)
{
  ASE_DEBUG_VARIANT_COUNT_DO(ase_variant_uengine_count_create++);
  std::auto_ptr<impl_type> im(new impl_type(cnf));
  impl = im.get(); /* set impl before creating script engines so that
    get_mutex() can be called from constrs */
  ase_script_engine_factory *const *sefs = &cnf.impl->servarr[0];
  const ase_size_type nsefs = cnf.impl->servarr.size();
  for (ase_size_type i = 0; i < nsefs; ++i) {
    ase_script_engine_factory *sef = sefs[i];
    ase_script_engine_ptr sc;
    sef->new_script_engine(*this, sc);
    im->engines.push_back(sc.get());
    sc.release();
  }
  impl->prepare_globals();
  const ase_variant host = ase_new_string_map();
  impl->aseglobal.set("Host", ase_string_map_unmodifiable(host));
  const ase_variant hargs = ase_new_array();
  host.set("Args", ase_array_unmodifiable(hargs));
  host.set("Array", ase_new_function_object(array_create()));
  host.set("StringMap", ase_new_function_object(strmap_create()));
  host.set("Load", ase_new_function_object(load_file_funcobj(this)));
  for (ase_size_type i = 0; i < cnf.impl->default_argv.size(); ++i) {
    const ase_int idx = static_cast<ase_int>(i);
    hargs.set(idx, cnf.impl->default_argv[i]);
  }
  im.release();
}

ase_unified_engine::~ase_unified_engine() /* DCHK */
{
  delete impl;
  ASE_DEBUG_VARIANT_COUNT_DO(ase_variant_uengine_count_destroy++);
}

void
ase_unified_engine::wait_threads()
{
  for (ase_size_type i = 0; i < impl->engines.size(); ++i) {
    impl->engines[i]->wait_threads();
  }
}

const char *const *
ase_unified_engine::get_path()
{
  return impl->conf.get_path();
}

ase_recursive_mutex&
ase_unified_engine::get_mutex()
{
  assert(impl);
  return impl->mutex;
}

ase_variant
ase_unified_engine::get_ase_global_object()
{
  return impl->aseglobal;
}

ase_script_engine *
ase_unified_engine::get_real_engine(const char *name)
{
  impl_type::global_info gl = impl->get_global(ase_string(name));
  if (gl.engine == 0) {
    return 0;
  }
  return gl.engine->get_real_engine();
}

namespace {

struct initthr {
  void operator ()(ase_script_engine *sc) {
    sc->child_thread_initialized();
  }
};

struct termthr {
  void operator ()(ase_script_engine *sc) {
    sc->child_thread_terminated();
  }
};

struct forcegc {
  void operator ()(ase_script_engine *sc) {
    sc->force_gc();
  }
};

}; // anonymous namespace

void
ase_unified_engine::child_thread_initialized()
{
  impl_type::engines_type& eng = impl->engines;
  ase_for_each(eng.begin(), eng.end(), initthr(), termthr());
}

void
ase_unified_engine::child_thread_terminated()
{
  impl_type::engines_type& eng = impl->engines;
  ase_for_each(eng.begin(), eng.end(), termthr());
}

void
ase_unified_engine::force_gc()
{
  ase_mutex_guard<ase_recursive_mutex> g(get_mutex());
  impl_type::engines_type& eng = impl->engines;
  ase_for_each(eng.begin(), eng.end(), forcegc());
}

ase_variant
ase_unified_engine::load_file(const ase_string& sfilename)
{
  ase_mutex_guard<ase_recursive_mutex> g(get_mutex());
  std::string filename(sfilename.data(), sfilename.size());
  std::string::size_type dot = filename.rfind('.');
  std::string base, suffix;
  if (dot != filename.npos) {
    base = filename.substr(0, dot);
    suffix = filename.substr(dot + 1);
  }
  ase_string hndlr = impl->conf.impl->find_handler(suffix);
  if (hndlr.empty()) {
    ase_string s = "ASERT: unsupported file type: " + suffix;
    ase_throw_invalid_arg(s.data());
  }
  impl_type::global_info gl = impl->get_global(hndlr);
  return gl.engine->load_file(hndlr.data(), sfilename.data());
}

bool
ase_runtime::impl_type::started() const
{
  return (rdllpath.size() != 0);
}

ase_string
ase_runtime::impl_type::find_dllfile(const ase_string& servicename) const
{
  if (dllpath.empty()) {
    return ase_string();
  }
  for (dllpath_type::size_type i = 0; i < dllpath.size(); ++i) {
    ase_string dlp = dllpath[i] + ASE_DIR_SEP_STR +
      get_dll_filename(servicename);
    if (ase_file_exists(dlp)) {
      ASE_VERBOSE_DELAY(fprintf(stderr, "file %s found\n", dlp.data()));
      return dlp;
    } else {
      ASE_VERBOSE_DELAY(fprintf(stderr, "file %s notfound\n", dlp.data()));
    }
  }
  return ase_string();
}

ase_runtime::ase_runtime(const char *argv0, int argc, char **argv,
  const ase_preexisting_engine_info *emb, size_t num_emb)
  : impl(0)
{
  std::auto_ptr<impl_type> im(new impl_type());
  impl = im.get();
  impl->init_args(ase_string(argv0), argc, argv);
  impl->set_preexisting_engines(emb, num_emb);
  impl->add_dllpath(get_lib_exec(argv0));
  impl->add_dllpath(ase_getcwd());
  impl->register_globals();
  impl->start(*this);
  im.release();
}

ase_runtime::~ase_runtime() /* DCHK */
{
  delete impl;
}

ase_string
ase_runtime::get_lib_exec(const char *argv0)
{
  const ase_string current_working_dir = ase_getcwd();
  ase_chdir_to_argv0(ase_string(argv0));
    /* chdir to the executable dir */
  const ase_string exec_dir = ase_getcwd();
    /* save exec dir */
  ase_chdir(current_working_dir);
    /* chdir back to the saved working dir */
  ase_string libexec_dir(ASE_DEFAULT_LIBEXEC);
  if (libexec_dir.empty()) {
    libexec_dir = exec_dir;
  }
  ase_string envle = ase_getenv_mtunsafe("ASE_LIBEXEC");
  if (!envle.empty()) {
    libexec_dir = envle;
  }
  return libexec_dir;
}

const char *const *
ase_runtime::get_path() const
{
  return &impl->rdllpath[0];
}

void *
ase_runtime::get_preexisting_engine(const char *name) const
{
  impl_type::preexisting_engines_type::const_iterator i =
    impl->preexisting_engines.find(std::string(name));
  if (i != impl->preexisting_engines.end()) {
    return i->second;
  }
  return 0;
}

void
ase_runtime::impl_type::start(ase_runtime& cnf)
{
  if (started()) {
    throw std::logic_error("already started");
  }
  for (dllpath_type::size_type j = 0; j < dllpath.size(); ++j) {
    rdllpath.push_back(dllpath[j].data());
  }
  rdllpath.push_back(0);
  confmap_type::const_iterator i;
  void *main_thread_id = ase_get_current_thread_id();
  for (i = confmap.begin(); i != confmap.end(); ++i) {
    const engine_props& p = i->second;
    std::vector<ase_string> gn;
    for (ase_size_type j = 0; j < p.globals.size(); ++j) {
      gn.push_back(p.globals[j].globalname);
    }
    const ase_string *const gnptr = gn.empty() ? 0 : &gn[0];
    ase_script_engine_factory_ptr fac;
    ase_new_delay_load_script_engine_factory(cnf, p.servicename, p.dllfilename,
      gnptr, gn.size(), main_thread_id, fac);
    servarr.push_back(fac.get());
    fac.release();
  }
}

void
ase_runtime::impl_type::register_global(const char *globalname_cstr,
  const char *servicename_cstr, const char *typestr_cstr)
{
  ase_string globalname(globalname_cstr);
  ase_string servicename(servicename_cstr);
  ase_string typestr(typestr_cstr);
  if (started()) {
    throw std::logic_error("already started");
  }
  engine_props& pr = confmap[servicename];
  if (pr.servicename.empty()) {
    pr.dllfilename = find_dllfile(servicename);
    pr.servicename = servicename;
  }
  typedef std::vector<global_props> vec_type;
  vec_type::size_type i;
  for (i = 0; i < pr.globals.size(); ++i) {
    global_props& glpr = pr.globals[i];
    if (glpr.globalname == globalname) {
      break;
    }
  }
  if (i == pr.globals.size()) {
    global_props glpr;
    glpr.globalname = globalname;
    pr.globals.push_back(glpr);
  }
  global_props& glpr = pr.globals[i];
  if (!typestr.empty()) {
    glpr.filetypes.push_back(typestr);
    handler_map_type::const_iterator i = handlermap.find(typestr);
    if (i == handlermap.end()) {
      handlermap[typestr] = handler_type(globalname);
    }
  }
}

void
ase_runtime::impl_type::init_args(const ase_string& argv0, int argc,
  char **argv)
{
  for (int i = 0; i < argc; ++i) {
    ase_string s(argv[i]);
    default_argv.push_back(s.to_variant());
  }
}

void
ase_runtime::impl_type::set_preexisting_engines(
  const ase_preexisting_engine_info *pe, size_t num_pe)
{
  for (size_t i = 0; i < num_pe; ++i) {
    preexisting_engines[std::string(pe[i].name)] = pe[i].pointer;
  }
}

void
ase_runtime::impl_type::add_dllpath(const ase_string& dname)
{
  if (std::find(dllpath.begin(), dllpath.end(), dname) == dllpath.end()) {
    ASE_VERBOSE_DELAY(fprintf(stderr, "ADD DLLPATH %s\n", dname.data()));
    dllpath.push_back(dname);
  }
}

void
ase_runtime::impl_type::register_globals()
{
  for (ase_size_type i = 0; i < dllpath.size(); ++i) {
    const ase_string& pt = dllpath[i];
    ase_string confdir = pt + ASE_DIR_SEP_STR + "aseconf_d";
    ase_variant ents = ase_read_directory(confdir);
    ase_int n = ents.get_length();
    for (ase_int j = 0; j < n; ++j) {
      ase_string nm = confdir + ASE_DIR_SEP_STR +
	ents.get_element(j).get_string();
      std::string sstr(nm.data());
      std::string::size_type dot = sstr.rfind('.');
      if (dot == sstr.npos || sstr.substr(dot + 1) != "aseconf") {
	continue;
      }
      ASE_VERBOSE_CONF(fprintf(stderr, "conffile: [%s]\n", nm.data()));
      std::ifstream fs(nm.data());
      std::string line, glname, svcname, suffix;
      while (std::getline(fs, line, '\n')) {
	if (line.size() > 0 && line[line.size() - 1] == '\r') {
	  line.resize(line.size() - 1);
	}
	if (parse_line(line, glname, svcname, suffix)) {
	  ASE_VERBOSE_CONF(fprintf(stderr, "register: %s\n", glname.c_str()));
	  register_global(glname.c_str(), svcname.c_str(), suffix.c_str());
	}
      }
    }
  }
}

ase_string
ase_runtime::impl_type::find_handler(const ase_string& ftypename) const
{
  handler_map_type::const_iterator i = handlermap.find(ftypename);
  if (i == handlermap.end()) {
    return ase_string();
  }
  return i->second.globalname;
}

void
ase_runtime::show_statistics()
{
  ASE_DEBUG_VARIANT_COUNT_DO(fprintf(stderr, "ENGINE   c = %d\n",
    (int)ase_variant_engine_count_create));
  ASE_DEBUG_VARIANT_COUNT_DO(fprintf(stderr, "ENGINE   d = %d\n",
    (int)ase_variant_engine_count_destroy));
  ASE_DEBUG_VARIANT_COUNT_DO(fprintf(stderr, "EFAC     c = %d\n",
    (int)ase_variant_efac_count_create));
  ASE_DEBUG_VARIANT_COUNT_DO(fprintf(stderr, "EFAC     d = %d\n",
    (int)ase_variant_efac_count_destroy));
  ASE_DEBUG_VARIANT_COUNT_DO(fprintf(stderr, "ESHARED  c = %d\n",
    (int)ase_variant_eshared_count_create));
  ASE_DEBUG_VARIANT_COUNT_DO(fprintf(stderr, "ESHARED  d = %d\n",
    (int)ase_variant_eshared_count_destroy));
  ASE_DEBUG_VARIANT_COUNT_DO(fprintf(stderr, "UENG     c = %d\n",
    (int)ase_variant_uengine_count_create));
  ASE_DEBUG_VARIANT_COUNT_DO(fprintf(stderr, "UENG     d = %d\n",
    (int)ase_variant_uengine_count_destroy));
}

