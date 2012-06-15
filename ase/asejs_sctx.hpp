
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEJS_SCTX_HPP
#define GENTYPE_ASEJS_SCTX_HPP

#include "asejs_aseimpl.hpp"

namespace asejs {

extern JSClass ase_js_aseproxy_class;
extern JSClass ase_js_asemethodproxy_class;

struct ase_js_error_report {

  ase_js_error_report()
    : has_error(false), is_outofmemory(false), is_runtimeerror(false),
      is_unknownerror(false), error_number(0), lineno(0), pos(0) { }

  void clear() {
    has_error = false;
    is_outofmemory = false;
    is_runtimeerror = false;
    is_unknownerror = false;
    error_number = 0;
    message.clear();
    filename.clear();
    lineno = 0;
    pos = 0;
  }

  bool has_error : 1;
  bool is_outofmemory : 1;
  bool is_runtimeerror : 1;
  bool is_unknownerror : 1;
  ase_size_type error_number;
  std::string message;
  std::string filename;
  ase_size_type lineno;
  ase_size_type pos;

};

struct ase_js_engine_factory;

struct ase_js_script_engine : public ase_script_engine {

  ase_js_script_engine(ase_unified_engine_ref& ue, ase_js_engine_factory *ef);
  ~ase_js_script_engine(); /* DCHK */
  void init();
  void cleanup();

  void destroy() {
    delete this;
  }
  void child_thread_initialized() { }
  void child_thread_terminated() { }
  void force_gc();
  void wait_threads() { }
  void dispose_proxies();
  const char *const *get_global_names();
  ase_variant get_global_object(const char *name);
  void import_ase_global(const ase_variant& val);
  ase_variant load_file(const char *glname, const char *filename);
  ase_script_engine *get_real_engine();

  void handle_js_error(JSContext *cx);
  void save_js_error(JSContext *cx, const char *message,
    JSErrorReport *report) throw();

  ase_js_aseproxy_privnode *get_ase_endnode() const { return ase_endnode; }
  ase_js_ase_variant_implnode *get_js_endnode() const { return js_endnode; }
  JSRuntime *get_js_runtime() const { return jsruntime; }
  JSObject *get_js_global() const { return jglobal; }
  const ase_variant_vtable *get_variant_vtable() const {
    return &variant_vtable; }
  const ase_variant_vtable *get_glfunc_vtable() const {
    return &glfunc_vtable; }
  ase_recursive_mutex& get_uemutex() const { return uemutex; }

  void get_pmc(PMC_APIFunctionTable_2 **pmctable_r, PMCPerl **pmcperl_r);
  PMC_APIFunctionTable_2 *get_pmctable();
  PMCPerl *get_pmcperl();

  JSContext *new_threadfree_jscontext();
  void destroy_threadfree_jscontext(JSContext *cx);

  JSObject *get_prototype(JSContext *cx, const ase_variant& v);

  jsrefcount suspend_request(JSContext *cx);
  void resume_request(JSContext *cx, jsrefcount depth);

 private:

  void begin_request(JSContext *cx);
  void end_request(JSContext *cx);
  static JSBool gc_callback(JSContext *cx, JSGCStatus status);
  void define_global_func(const char *name,
    ase_js_ase_variant_glfunc_impl::funcptr_type funcptr);

  ase_variant get_real_plglobal();
  static ase_variant get_real_plglobal_cb(const ase_string& name,
    void *userdata);

 private:

  typedef std::vector<JSContext *> jsctx_pool_type;
  typedef std::vector<ase_js_ase_variant_glfunc_impl *> glfuncs_type;
  typedef std::map<const void *, JSObject *> klassmap_type;

  ase_unified_engine_ref& ueref;
  ase_recursive_mutex& uemutex;
  bool js_preemptive;
  ase_variant_vtable variant_vtable;
  ase_variant_vtable glfunc_vtable;
  JSRuntime *jsruntime;
  JSContext *default_cx;
  ase_js_engine_factory *backref;

  ase_fast_mutex pmcperl_mutex;
  PMC_APIFunctionTable_2 *pmctable_cur;
  PMCPerl *pmcperl_cur;
  bool pmcperl_init;

  jsctx_pool_type jsctx_pool;
  ase_variant global;
  JSObject *jglobal;
  ase_variant plglobal;
  ase_variant ase_global;
  JSObject *klasses_root;
  klassmap_type klassmap;
  JSObject *aseproxy_proto;
  ase_js_ase_variant_implnode *js_endnode; /* js proxy list */
  ase_js_aseproxy_privnode *ase_endnode;   /* ase proxy list */
  glfuncs_type glfuncs; /* elements are non-owning pointers */
  ase_js_error_report js_error;

 private:

  static ase_atomic_count script_engine_count;

};

class ase_js_scoped_context : public ase_noncopyable {

 public:

  ase_js_scoped_context(ase_js_script_engine *sc)
    : script_engine(sc), cx(0) {
    cx = script_engine->new_threadfree_jscontext();
  }
  ~ase_js_scoped_context() /* DCHK */ {
    script_engine->destroy_threadfree_jscontext(cx);
  }
  JSContext *get() const { return cx; }

 private:

  ase_js_script_engine *script_engine;
  JSContext *cx;

};

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4311 4312 )
#endif

inline JSString *jsval_to_string(const jsval& v) {
  return JSVAL_TO_STRING(v);
}

inline jsval double_to_jsval(jsdouble *dp) {
  return DOUBLE_TO_JSVAL(dp);
}

inline jsval string_to_jsval(JSString *s) {
  return STRING_TO_JSVAL(s);
}

inline JSObject *jsval_to_object(const jsval& v) {
  return JSVAL_TO_OBJECT(v);
}

inline jsdouble *jsval_to_double(const jsval& v) {
  return JSVAL_TO_DOUBLE(v);
}

inline jsval object_to_jsval(JSObject *o) {
  return OBJECT_TO_JSVAL(o);
}

inline bool jsval_is_nonnull_object(const jsval& v) {
  return JSVAL_IS_OBJECT(v) && JSVAL_TO_OBJECT(v) != NULL;
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

}; // namespace asejs

#endif

