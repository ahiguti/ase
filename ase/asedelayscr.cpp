
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asedelayscr.hpp>
#include <ase/asedelayvar.hpp>
#include <ase/asedllhandle.hpp>
#include <ase/asemutex.hpp>
#include <ase/aseatomic.hpp>
#include <ase/aseflags.hpp>
#include <ase/aseio.hpp>
#include <ase/aseexcept.hpp>

#include <map>
#include <vector>
#include <memory>

#define DBG_UNKNOWN(x)
#define DBG_GLOBAL(x)

namespace {

struct ase_delay_engine_factory;
struct ase_delay_script_engine;

struct engine_props {
  ase_string servicename;
  ase_string dllfilename;
  std::vector<ase_string> globals;
};

struct ase_delay_script_engine : public ase_script_engine {

  ase_delay_script_engine(ase_delay_engine_factory *svc, void *mainthr,
    ase_unified_engine_ref& ue);
  void destroy() { delete this; }
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

  ase_variant get_real_global(const ase_string& name);

 private:

  struct global_type {
    ase_string name;
    ase_variant value;
  };
  typedef std::vector<global_type> globals_type;

  ase_atomic_count refcount;
  ase_delay_engine_factory *service_backref;
  void *main_thread_id;
  ase_unified_engine_ref& ueref;
  ase_script_engine_ptr real_engine;
  globals_type my_globals;
  ase_variant aseglobal;

 private:

  static ase_variant get_real_global_cb(const ase_string& name,
    void *userdata);

};

struct ase_delay_engine_factory : public ase_script_engine_factory {

  void destroy() { delete this; }
  void new_script_engine(ase_unified_engine_ref& ue,
    ase_script_engine_ptr& ap_r);

  ase_delay_engine_factory(ase_environment& env, const ase_string& servicename,
    const ase_string& dllfilename, const ase_string *globalnames,
    ase_size_type globalnames_num, void *mainthr);
  void create_real_engine(ase_unified_engine_ref& ue,
    ase_script_engine_ptr& ap_r);
  const char *const *get_global_names() const;
  const engine_props& get_props() const { return props; }

  static ase_delay_engine_factory *get(void *priv);

  ase_fast_mutex efac_mutex;
  ase_environment& env;
  engine_props props;
  std::vector<const char *> global_names;
  std::auto_ptr<ase_auto_dll_handle> dllhandle;
  ase_script_engine_factory_ptr real_service;
  void *main_thread_id;

};

ase_delay_script_engine::ase_delay_script_engine(
  ase_delay_engine_factory *svc, void *mainthr, ase_unified_engine_ref& ue)
  : refcount(1), service_backref(svc), main_thread_id(mainthr),
    ueref(ue)
{
  const engine_props& props = svc->get_props();
  typedef std::vector<ase_string>::size_type size_type;
  size_type n = props.globals.size();
  for (size_type i = 0; i < n; ++i) {
    global_type gl;
    gl.name = props.globals[i];
    gl.value = ase_new_delay_load_variant(ue, get_real_global_cb, gl.name,
      this);
    my_globals.push_back(gl);
  }
}

void
ase_delay_script_engine::child_thread_initialized()
{
  ase_mutex_guard<ase_recursive_mutex> g(ueref.get_mutex());
  if (real_engine.get()) {
    real_engine->child_thread_initialized();
  }
}

void
ase_delay_script_engine::child_thread_terminated()
{
  ase_mutex_guard<ase_recursive_mutex> g(ueref.get_mutex());
  if (real_engine.get()) {
    real_engine->child_thread_terminated();
  }
}

void
ase_delay_script_engine::force_gc()
{
  ase_mutex_guard<ase_recursive_mutex> g(ueref.get_mutex());
  if (real_engine.get()) {
    real_engine->force_gc();
  }
}

void
ase_delay_script_engine::wait_threads()
{
  /* wait_threads must not lock ueref */
  my_globals.clear();
  aseglobal = ase_variant();
  if (real_engine.get()) {
    real_engine->wait_threads();
  }
}

void
ase_delay_script_engine::dispose_proxies()
{
  ase_mutex_guard<ase_recursive_mutex> g(ueref.get_mutex());
  my_globals.clear();
  aseglobal = ase_variant();
  if (real_engine.get()) {
    real_engine->dispose_proxies();
  }
}

const char *const *
ase_delay_script_engine::get_global_names()
{
  return service_backref->get_global_names();
}

ase_variant
ase_delay_script_engine::get_global_object(const char *name)
{
  ase_string n(name);
  typedef std::vector<ase_string>::size_type size_type;
  for (size_type i = 0; i < my_globals.size(); ++i) {
    const global_type& gl = my_globals[i];
    if (gl.name == n) {
      DBG_UNKNOWN(fprintf(stderr, "returning global: %s\n", name));
      return gl.value;
    }
  }
  DBG_UNKNOWN(fprintf(stderr, "unknown global: %s\n", name));
  return ase_variant();
}

void
ase_delay_script_engine::import_ase_global(const ase_variant& val)
{
  aseglobal = val;
}

ase_variant
ase_delay_script_engine::load_file(const char *glname, const char *filename)
{
  get_real_global(ase_string(glname));
  ase_mutex_guard<ase_recursive_mutex> g(ueref.get_mutex());
  if (real_engine.get()) {
    return real_engine->load_file(glname, filename);
  } else {
    return ase_variant();
  }
}

ase_script_engine *
ase_delay_script_engine::get_real_engine()
{
  ase_mutex_guard<ase_recursive_mutex> g(ueref.get_mutex());
  if (!real_engine.get()) {
    void *cur_thread_id = ase_get_current_thread_id();
    if (cur_thread_id != main_thread_id) {
      /* delay loading must not occur in the multi-threaded state */
      throw std::logic_error("(ASELE) DelayLoad "
	": must be created from the main thread");
    }
    ase_script_engine_ptr eng;
    service_backref->create_real_engine(ueref, eng);
    eng->import_ase_global(aseglobal);
    real_engine = eng;
  }
  return real_engine.get();
}

ase_variant
ase_delay_script_engine::get_real_global(const ase_string& name)
{
  this->get_real_engine();
  ASE_VERBOSE_DELAY(fprintf(stderr, "getreal %s\n", name.data()));
  return real_engine->get_global_object(name.data());
}

ase_variant
ase_delay_script_engine::get_real_global_cb(const ase_string& name,
  void *userdata)
{
  ase_delay_script_engine *engine_backref =
    static_cast<ase_delay_script_engine *>(userdata);
  ASE_VERBOSE_DELAY(fprintf(stderr, "LOAD GLOBAL %s\n", name.data()));
  return engine_backref->get_real_global(name);
}

ase_delay_engine_factory *
ase_delay_engine_factory::get(void *priv)
{
  return static_cast<ase_delay_engine_factory *>(priv);
}

ase_string
get_dll_filename(const ase_string& servicename)
{
  return ase_string("libase-scr-") + servicename + ASE_DLL_SUFFIX;
}

void
ase_delay_engine_factory::create_real_engine(ase_unified_engine_ref& ue,
  ase_script_engine_ptr& ap_r)
{
  ase_mutex_guard<ase_fast_mutex> g(efac_mutex);
  if (!real_service.get()) {
    if (props.dllfilename.empty()) {
      const char *const *path = env.get_path();
      ase_string s = "ASE: failed to load "
	+ get_dll_filename(props.servicename);
      while (path && (*path)) {
	ase_string dlp = ase_string(*path)
	  + ASE_DIR_SEP_STR + get_dll_filename(props.servicename);
	s = s + "\n\t(tried: " + dlp + ")";
	++path;
      }
      ase_throw_io_error(s.data());
    }
    ASE_VERBOSE_DELAY(fprintf(stderr, "dllfilename=%s\n",
      props.dllfilename.data()));
    ASE_VERBOSE_DELAY(fprintf(stderr, "servicename=%s\n",
      props.servicename.data()));
    dllhandle.reset(new ase_auto_dll_handle(props.dllfilename, true));
    if (!dllhandle->get_error().empty()) {
      ase_string s = "ASE: DLLOpen: " + dllhandle->get_error();
      ase_throw_io_error(s.data());
    }
    ASE_VERBOSE_DELAY(fprintf(stderr, "dll open\n"));
    ase_string n = "ASE_NewScriptEngineFactory_" + props.servicename;
    void *sym = dllhandle->get_symbol(n);
    if (!sym) {
      ase_string s = "ASE: get_symbol: " + dllhandle->get_error();
      ase_throw_invalid_arg(s.data());
    }
    typedef ase_new_script_engine_factory_func *func_t;
    func_t f = (func_t)sym;
    ase_script_engine_factory_ptr asvc;
    f(env, asvc);
    real_service = asvc;
  }
  real_service->new_script_engine(ue, ap_r);
}

const char *const *
ase_delay_engine_factory::get_global_names() const
{
  return &global_names[0];
}

void
ase_delay_engine_factory::new_script_engine(ase_unified_engine_ref& ue,
  ase_script_engine_ptr& ap_r)
{
  ap_r.reset(new ase_delay_script_engine(this, main_thread_id, ue));
}

ase_delay_engine_factory::ase_delay_engine_factory(ase_environment& e,
  const ase_string& servicename, const ase_string& dllfilename,
  const ase_string *globalnames, ase_size_type globalnames_num, void *mainthr)
  : env(e), props(), real_service(0), main_thread_id(mainthr)
{
  props.servicename = servicename;
  props.dllfilename = dllfilename;
  for (ase_size_type i = 0; i < globalnames_num; ++i) {
    props.globals.push_back(globalnames[i]);
  }
  typedef std::vector<ase_string>::size_type size_type;
  size_type n = props.globals.size();
  global_names.resize(n + 1);
  for (size_type i = 0; i < n; ++i) {
    global_names[i] = props.globals[i].data();
    DBG_GLOBAL(fprintf(stderr, "delay global: %p %s\n", this,
      global_names[i]));
  }
  global_names[n] = 0;
}

}; // anonymous namespace

void
ase_new_delay_load_script_engine_factory(ase_environment& e,
  const ase_string& servicename, const ase_string& dllfilename,
  const ase_string *globalnames, ase_size_type globalnames_num,
  void *mainthr, ase_script_engine_factory_ptr& ap_r)
{
  ap_r.reset(new ase_delay_engine_factory(e, servicename, dllfilename,
    globalnames, globalnames_num, mainthr));
}

