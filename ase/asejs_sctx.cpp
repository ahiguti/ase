
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/aseexcept.hpp>
#include <ase/asedelayvar.hpp>
#include <ase/aseutf16.hpp>
#include <ase/asecast.hpp>
#include <ase/aseio.hpp>
#include <ase/asefuncobj.hpp>

#include "asejs_sctx.hpp"
#include "asejs_asepr.hpp"
#include "asejs_util.hpp"

#include <cassert>
#include <malloc.h> // glibc mallinfo()

#define DBG_SC(x)
#define DBG_POOL(x)
#define DBG_CTHR(x)
#define DBG_CHECKERR(x)
#define DBG_PCACHE(x)
#define DBG_DISPOSE(x)
#define DBG_KLASSMAP(x)
#define DBG_IMPORT(x)
#define DBG_ERR_ABORT(x)
#define DBG_ERR(x)
#define DBG_OOM_ABORT(x)
#define DBG_STAT(x)
#define DBG_THR(x)
#define DBG_GC(x)
#define DBG_GCMETER(x)
#define DBG_PMC(x)

#ifdef __GNUC__
#define JS_GCMETER
#include <jsgc.h>
#include <jscntxt.h>
#endif

namespace asejs {

JSClass ase_js_aseproxy_class = {
  "ASEVariant", JSCLASS_HAS_PRIVATE,
  JS_PropertyStub, ase_js_aseproxy_delprop, ase_js_aseproxy_getprop,
  ase_js_aseproxy_setprop, ase_js_aseproxy_enumerate, JS_ResolveStub,
  ase_js_aseproxy_convert, ase_js_aseproxy_finalize,
  // 0, 0, 0, 0, 0, 0, 0, 0 
  0, 0, ase_js_aseproxy_call, 0, 0, 0, 0, 0 
};

JSClass ase_js_asemethodproxy_class = {
  "ASEVariantMethod", JSCLASS_HAS_PRIVATE,
  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
  JS_PropertyStub, JS_EnumerateStub, JS_ResolveStub,
  ase_js_asemethodproxy_convert, JS_FinalizeStub,
  0, 0, ase_js_asemethodproxy_call, 0, 0, 0, 0, 0 
};

JSPropertySpec ase_js_aseproxy_props[] = {
  { 0, 0, 0, 0, 0 }
};

JSPropertySpec ase_js_aseproxy_static_props[] = {
  { 0, 0, 0, 0, 0 }
};

JSFunctionSpec ase_js_aseproxy_static_funcs[] = {
  { 0, 0, 0, 0, 0 },
};

JSFunctionSpec ase_js_aseproxy_methods[] = {
  { "toString", ase_js_aseproxy_to_string, 0, 0, 0 },
  { 0, 0, 0, 0, 0 },
};

JSBool
ase_js_perlstub_getperl(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
  /* called when 'Perl' is read from JS code */
  ase_js_script_engine *sc = static_cast<ase_js_script_engine *>(
    JS_GetContextPrivate(cx));
  assert(sc);
  /* remove myself */
  if (!JS_DeleteProperty(cx, sc->get_js_global(), "Perl")) {
    return JS_FALSE;
  }
  /* initialize pmcperl */
  PMC_APIFunctionTable_2 *pmctable = 0;
  PMCPerl *pmcperl = 0;
  try {
    sc->get_pmc(&pmctable, &pmcperl);
  } catch (...) {
    ase_js_util::handle_exception(sc, cx);
    return JS_FALSE;
  }
  return JS_GetProperty(cx, sc->get_js_global(), "Perl", vp);
}

JSBool
ase_js_perlstub_setperl(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
  return JS_TRUE;
}

void
ase_js_report_error(JSContext *cx, const char *message, JSErrorReport *report)
{
  DBG_ERR(fprintf(stderr, "js error: %s\n", message));
  DBG_ERR_ABORT(::abort());
  ase_js_script_engine *sc = static_cast<ase_js_script_engine *>(
    JS_GetContextPrivate(cx));
  assert(sc);
  sc->save_js_error(cx, message, report);
}

ase_js_script_engine::ase_js_script_engine(ase_unified_engine_ref& ue,
  ase_js_engine_factory *ef)
  : ueref(ue), uemutex(ue.get_mutex()), js_preemptive(false),
    variant_vtable(ase_variant_impl<ase_js_ase_variant_impl>::vtable),
    glfunc_vtable(ase_variant_impl<ase_js_ase_variant_glfunc_impl>::vtable),
    jsruntime(0), default_cx(0), backref(ef),
    pmctable_cur(0), pmcperl_cur(0),
    pmcperl_init(false),
    jglobal(0), klasses_root(0), aseproxy_proto(0), js_endnode(0),
    ase_endnode(0)
{
  try {
    init();
  } catch (const std::bad_alloc&) {
    cleanup();
    throw;
  } catch (...) {
    cleanup();
    /* we must not rethrow the thrown object because it may refer to
       'this'. */
    ase_throw_error("ASEJS: failed to initialize js engine");
  }
}

ase_js_script_engine::~ase_js_script_engine() /* DCHK */
{
  cleanup();
}

void
ase_js_script_engine::init()
{
  script_engine_count++;
  variant_vtable.impl_private_data = this;
  glfunc_vtable.impl_private_data = this;
  jsruntime = JS_NewRuntime(64L * 1024L * 1024L);
  if (!jsruntime) {
    handle_js_error(default_cx);
    return;
  }
  JS_SetRuntimePrivate(jsruntime, this);
  JS_SetGCCallbackRT(jsruntime, &ase_js_script_engine::gc_callback);
  default_cx = JS_NewContext(jsruntime, 8L * 1024L);
  if (!default_cx) {
    handle_js_error(default_cx);
    return;
  }
  JS_SetContextPrivate(default_cx, this);
  /* don't do BeginRequest on default_cx. */
  jglobal = JS_NewObject(default_cx, 0, 0, 0);
  if (!jglobal) {
    handle_js_error(default_cx);
    return;
  }
  klasses_root = JS_NewArrayObject(default_cx, 0, 0);
  if (!klasses_root) {
    handle_js_error(default_cx);
    return;
  }
  if (!JS_DefineProperty(default_cx, jglobal, "__ase_klasses__",
    object_to_jsval(klasses_root), 0, 0,
    JSPROP_READONLY | JSPROP_PERMANENT)) {
    handle_js_error(default_cx);
    return;
  }
  JS_SetGlobalObject(default_cx, jglobal);
  if (!JS_InitStandardClasses(default_cx, jglobal)) {
    handle_js_error(default_cx);
    return;
  }
  JS_SetErrorReporter(default_cx, ase_js_report_error);
  aseproxy_proto = JS_InitClass(default_cx, jglobal, 0,
    &ase_js_aseproxy_class,
    ase_js_aseproxy_constr, 0, ase_js_aseproxy_props, ase_js_aseproxy_methods,
    ase_js_aseproxy_static_props, ase_js_aseproxy_static_funcs);
  if (!aseproxy_proto) {
    handle_js_error(default_cx);
    return;
  }
  JSObject *asemethodproxy_proto = JS_InitClass(default_cx, jglobal, 0,
    &ase_js_asemethodproxy_class, 0, 0, 0, 0, 0, 0);
  if (!asemethodproxy_proto) {
    handle_js_error(default_cx);
    return;
  }
  js_endnode = new ase_js_ase_variant_implnode(
    ase_js_ase_variant_impl::init_arg(this, default_cx, 0, 0));
  ase_endnode = new ase_js_aseproxy_privnode(
    ase_js_aseproxy_priv::init_arg(this, ase_variant()));
  global = ase_js_util::to_ase_variant(this, default_cx,
    object_to_jsval(jglobal));
  define_global_func("New", ase_js_ase_variant_glfunc_new);
  define_global_func("EvalString", ase_js_ase_variant_glfunc_eval);
  define_global_func("ForceGC", ase_js_ase_variant_glfunc_gc);
  define_global_func("print", ase_js_ase_variant_glfunc_print);
  plglobal = ase_new_delay_load_variant(ueref, get_real_plglobal_cb,
    ase_string("Perl"), this);
  if (!JS_DefineProperty(default_cx, jglobal, "Perl", JSVAL_VOID,
    ase_js_perlstub_getperl, ase_js_perlstub_setperl, 0)) {
    handle_js_error(default_cx);
    return;
  }
}

void
ase_js_script_engine::cleanup()
{
  dispose_proxies();
  DBG_SC(fprintf(stderr, "destroy script context\n"));
  global = ase_variant();
  plglobal = ase_variant();
  DBG_POOL(fprintf(stderr, "pooled jsctx %d\n", (int)jsctx_pool.size()));
  for (jsctx_pool_type::size_type i = 0; i < jsctx_pool.size(); ++i) {
    JSContext *cx = jsctx_pool[i];
    JS_SetContextThread(cx);
    JS_DestroyContextNoGC(cx);
  }
  delete js_endnode;
  delete ase_endnode;
  if (pmcperl_cur) {
    DBG_PMC(fprintf(stderr, "destroy pmcperl %p\n", pmcperl));
    pmctable_cur->PMC_DestroyPerl(pmcperl_cur, default_cx);
  }
  if (default_cx) {
    /* don't do EndRequest on default_cx */
    JS_DestroyContext(default_cx);
    default_cx = 0;
  }
  if (jsruntime) {
    JS_DestroyRuntime(jsruntime);
    jsruntime = 0;
  }
  script_engine_count--;
  DBG_SC(fprintf(stderr, "destroy script context done\n"));
}

ase_variant
ase_js_script_engine::get_real_plglobal()
{
  /* initialize pmcperl */
  PMC_APIFunctionTable_2 *pmctable = 0;
  PMCPerl *pmcperl = 0;
  get_pmc(&pmctable, &pmcperl);
  /* get perl global */
  ase_variant pl = global.get("Perl");
  return pl;
}

ase_variant
ase_js_script_engine::get_real_plglobal_cb(const ase_string& name,
  void *userdata)
{
  ase_js_script_engine *sc = static_cast<ase_js_script_engine *>(userdata);
  return sc->get_real_plglobal();
}

void
ase_js_script_engine::get_pmc(PMC_APIFunctionTable_2 **pmctable_r,
  PMCPerl **pmcperl_r)
{
  (*pmctable_r) = 0;
  (*pmcperl_r) = 0;
  ase_mutex_guard<ase_fast_mutex> g(pmcperl_mutex);
  if (pmcperl_init) {
    (*pmctable_r) = pmctable_cur;
    (*pmcperl_r) = pmcperl_cur;
    return;
  }
  pmctable_cur = backref->getPMCTable();
  if (pmctable_cur) {
    pmcperl_cur = pmctable_cur->PMC_NewPerl();
    if (!pmcperl_cur) {
      handle_js_error(default_cx);
      return;
    }
    DBG_PMC(fprintf(stderr, "new pmcperl %p\n", pmcperl));
    JSObject *pmcobj = pmctable_cur->PMC_InitClasses(pmcperl_cur, default_cx,
      jglobal);
    if (!pmcobj) {
      handle_js_error(default_cx);
      return;
    }
    /* set 'JS' on perl */
    ase_variant pl = global.get("Perl");
    ase_variant plmainpkg = pl.get("Scalars").get("main");
    plmainpkg.set("JS", global);
    /* import ext globals */
    plmainpkg.set("ASE", ase_global);
  }
  pmcperl_init = true;
  (*pmctable_r) = pmctable_cur;
  (*pmcperl_r) = pmcperl_cur;
}

PMC_APIFunctionTable_2 *
ase_js_script_engine::get_pmctable()
{
  PMC_APIFunctionTable_2 *pmctable = 0;
  PMCPerl *pmcperl = 0;
  get_pmc(&pmctable, &pmcperl);
  return pmctable;
}

PMCPerl *
ase_js_script_engine::get_pmcperl()
{
  PMC_APIFunctionTable_2 *pmctable = 0;
  PMCPerl *pmcperl = 0;
  get_pmc(&pmctable, &pmcperl);
  return pmcperl;
}

void
ase_js_script_engine::define_global_func(const char *name,
  ase_js_ase_variant_glfunc_impl::funcptr_type funcptr)
{
  glfuncs.push_back(new ase_js_ase_variant_glfunc_impl(this, default_cx,
    funcptr));
  ase_variant fv = ase_variant::create_object(&glfunc_vtable, glfuncs.back(),
    "JSGLFunc");
  global.set_property(name, std::strlen(name), fv);
  JSBool found = JS_TRUE;
  JS_SetPropertyAttributes(default_cx, jglobal, name,
    JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT, &found);
}

JSObject *
ase_js_script_engine::get_prototype(JSContext *cx, const ase_variant& v)
{
  const void *vtbl = v.get_vtable_address();
  const ase_size_type num = v.get_num_methods();
  const ase_variant_method_entry *mtds = v.get_methods();
  if (num == 0) {
    return 0;
  }
  ase_mutex_guard<ase_recursive_mutex> g(get_uemutex());
  klassmap_type::const_iterator itr = klassmap.find(vtbl);
  if (itr != klassmap.end()) {
    DBG_KLASSMAP(fprintf(stderr, "klassmap: found %p\n", vtbl));
    return itr->second;
  }
  DBG_KLASSMAP(fprintf(stderr, "klassmap: create %p\n", vtbl));
  jsuint len = 0;
  if (!JS_GetArrayLength(cx, klasses_root, &len)) {
    handle_js_error(cx);
    return 0;
  }
  JSObject *proto = JS_NewObject(cx, 0, aseproxy_proto, 0);
  if (!proto) {
    handle_js_error(cx);
    return 0;
  }
  DBG_KLASSMAP(fprintf(stderr, "klassmap: %p proto=%p\n", vtbl, proto));
  jsval protoval = object_to_jsval(proto);
  if (!JS_SetElement(cx, klasses_root, static_cast<jsint>(len), &protoval)) {
    handle_js_error(cx);
    return 0;
  }
  for (ase_size_type i = 0; i < num; ++i) {
    const ase_variant_method_entry& ent = mtds[i];
    const char *const name = ent.u8name;
    const int namelen = ase_numeric_cast<int>(ent.u8name_len);
    if (std::strcmp(name, "toString") == 0) {
      /* 'toString' is overriden by ase_variant::get_string() */
      if (!JS_DefineFunction(cx, proto, "toString", ase_js_aseproxy_to_string,
	0, 0)) {
	handle_js_error(cx);
	return 0;
      }
      continue;
    }
    ase_utf16_string namestr(name, namelen);
    JSObject *fo = JS_NewObject(cx, &ase_js_asemethodproxy_class, 0, 0);
    if (!fo) {
      handle_js_error(cx);
      return 0;
    }
    /* private data must be 2byte aligned. x8 for safety. */
    void *priv = reinterpret_cast<void *>(static_cast<ase_size_type>(i * 8));
    if (!JS_SetPrivate(cx, fo, priv)) {
      handle_js_error(cx);
      return 0;
    }
    DBG_KLASSMAP(fprintf(stderr, "klassmap: method %s mpobj=%p num=%i\n",
      name, fo, (int)i));
    if (!JS_DefineUCProperty(cx, proto, ase_js_util::to_jschar(namestr.data()),
      namestr.size(), object_to_jsval(fo), 0, 0,
      JSPROP_READONLY | JSPROP_PERMANENT)) {
      handle_js_error(cx);
      return 0;
    }
  }
  klassmap[vtbl] = proto;
  return proto;
}

void
ase_js_script_engine::force_gc()
{
  ase_js_scoped_context sctx(this);
  JSContext *const cx = sctx.get();
  JS_GC(cx);
}

void
ase_js_script_engine::dispose_proxies()
{
  if (!default_cx) {
    return;
  }
  if (js_endnode) {
    ase_js_ase_variant_implnode *p = js_endnode;
    do {
      DBG_DISPOSE(fprintf(stderr, "dispose vimpl %p\n", p));
      p->value.dispose(this);
      p = p->get_next();
    } while (p != js_endnode);
  }
  if (ase_endnode) {
    ase_js_aseproxy_privnode *p = ase_endnode;
    do {
      p->value.dispose(this);
      p = p->get_next();
    } while (p != ase_endnode);
  }
  /* glfunc elements are non-owning pointers. glfuncs are freed by above
    dispose() */
  glfuncs.clear();
  ase_global = ase_variant();
  /* detach pmcperl proxies */
  {
    ase_mutex_guard<ase_fast_mutex> g(pmcperl_mutex);
    if (pmcperl_cur) {
      pmctable_cur->PMC_DetachProxies(pmcperl_cur, default_cx);
    }
  }
}

const char *const *
ase_js_script_engine::get_global_names()
{
  static const char *const globals[] = { "JS", "Perl", 0 };
  return globals;
}

ase_variant
ase_js_script_engine::get_global_object(const char *name)
{
  std::string s(name);
  if (s == "JS") {
    return global;
  } else if (s == "Perl") {
    return plglobal;
  }
  return ase_variant();
}

void
ase_js_script_engine::import_ase_global(const ase_variant& val)
{
  ase_mutex_guard<ase_recursive_mutex> g(uemutex); /* not necessary */
  DBG_IMPORT(fprintf(stderr, "js: import %s\n", name));
  std::string s("ASE");
  global.set_property(s.data(), s.size(), val);
  /* set ase_global so that we can import them to perl later. */
  ase_global = val;
}

namespace {

struct loadfile_fobj {
  ase_variant func;
  ase_variant arg;
  ase_variant operator ()(const ase_variant *args, ase_size_type nargs) {
    return func.invoke(arg);
  }
};

}; // anonymous namespace

ase_variant
ase_js_script_engine::load_file(const char *glname, const char *filename)
{
  ase_mutex_guard<ase_recursive_mutex> g(uemutex); /* not necessary */
  ase_string str = ase_load_file(ase_string(filename));
  ase_variant gl = global;
  if (ase_string::equals(glname, std::strlen(glname), "Perl")) {
    gl = plglobal;
  }
  ase_variant func, e;
  try {
    func = gl.get("EvalString");
  } catch (const ase_variant& ex) {
    e = ex;
  } catch (const std::runtime_error& ex) {
    e = ase_string(ex.what()).to_variant();
  }
  if (func.is_void()) {
    ase_string s = "ASEJS: script engine not found: " + ase_string(glname);
    ase_throw_error(s.data());
  }
  return func.invoke(str.to_variant());
}

ase_script_engine *
ase_js_script_engine::get_real_engine()
{
  return this;
}

void
ase_js_script_engine::save_js_error(JSContext *cx, const char *message,
  JSErrorReport *report) throw()
{
  DBG_CHECKERR(fprintf(stderr, "save_js_error: %s\n", message));
  ase_js_error_report& err = js_error;
  if (err.has_error) {
    /* don't overwrite */
    return;
  }
  try {
    err.clear();
    err.has_error = true;
    err.error_number = static_cast<ase_size_type>(report->errorNumber);
    err.is_outofmemory = (err.error_number == 137); /* js.msg */
    if (!err.is_outofmemory) {
      err.is_runtimeerror = true;
    }
    err.message = std::string(message);
    const char *const fname = report->filename;
    if (fname) {
      err.filename = std::string(fname);
    }
    err.lineno = static_cast<ase_size_type>(report->lineno);
    if (report->tokenptr && report->linebuf) {
      err.pos = static_cast<ase_size_type>(report->tokenptr - report->linebuf);
    }
  } catch (const std::bad_alloc&) {
    err.is_outofmemory = true;
  } catch (...) {
    err.is_unknownerror = true;
  }
  if (err.is_outofmemory) {
    DBG_OOM_ABORT(struct mallinfo mi = ::mallinfo());
    DBG_OOM_ABORT(fprintf(stderr, "uordblks: %d\n", mi.uordblks));
    DBG_OOM_ABORT(::abort());
  }
}

void
ase_js_script_engine::handle_js_error(JSContext *cx)
{
  DBG_CHECKERR(fprintf(stderr, "handle_js_error\n"));
  /* it's possible that cx is null. */
  if (cx) {
    ase_js_error_report err = js_error; /* copy */
    js_error.clear();
    if (err.has_error) {
      if (err.is_outofmemory) {
	throw std::bad_alloc();
      }
      ase_string mess = ase_string("(ASEJS) ") + err.message;
      if (!err.filename.empty()) {
	ase_variant ln(static_cast<ase_long>(err.lineno));
	mess = mess + " (" + err.filename + ":" + ln.get_string() + ")";
      }
      if (err.is_runtimeerror) {
	ase_variant excval = ase_new_exception(ase_string("JSError"), mess);
	throw excval;
      } else {
	ase_variant excval = ase_new_exception(ase_string("JSError"), mess);
	throw excval;
      }
    }
    jsval jsex = JSVAL_VOID;
    if (!JS_GetPendingException(cx, &jsex)) {
      DBG_CHECKERR(fprintf(stderr, "handle_js_error: no pending exception\n"));
      ase_throw_error("ASEJS: failed to get pending exception");
    }
    DBG_CHECKERR(ase_js_util::ase_js_dump_jsval(cx, jsex));
    ase_variant av = ase_js_util::to_ase_variant(this, cx, jsex);
    DBG_CHECKERR(fprintf(stderr, "handle_js_error: %s\n",
      av.get_string().toUTF8().c_str()));
    JS_ClearPendingException(cx);
    throw av;
  }
  ase_throw_error("ASEJS: unknown error");
}

JSBool
ase_js_branch_callback(JSContext *cx, JSScript *script)
{
  static unsigned int volatile branch_count = 0; /* need not to synchronize */
  if (branch_count++ > 10000) {
    branch_count -= 10000;
    DBG_STAT(struct mallinfo mi = ::mallinfo());
    DBG_STAT(fprintf(stderr, "uordblks: %d\n", mi.uordblks));
//    jsrefcount depth = JS_SuspendRequest(cx);
//    JS_ResumeRequest(cx, depth);
  }
  return JS_TRUE;
}

jsrefcount
ase_js_script_engine::suspend_request(JSContext *cx)
{
  DBG_THR(fprintf(stderr, "suspend_request%p\n", this));
  if (js_preemptive) {
    return JS_SuspendRequest(cx);
  } else {
    uemutex.unlock();
    return 0;
  }
}

void
ase_js_script_engine::resume_request(JSContext *cx, jsrefcount depth)
{
  if (js_preemptive) {
    JS_ResumeRequest(cx, depth);
  } else {
    uemutex.lock();
  }
  DBG_THR(fprintf(stderr, "resume_request%p\n", this));
}

void
ase_js_script_engine::begin_request(JSContext *cx)
{
  if (js_preemptive) {
    JS_BeginRequest(cx);
  } else {
    uemutex.lock();
  }
  DBG_THR(fprintf(stderr, "begin_request %p\n", this));
}

void
ase_js_script_engine::end_request(JSContext *cx)
{
  DBG_THR(fprintf(stderr, "end_request %p\n", this));
  if (js_preemptive) {
    JS_EndRequest(cx);
  } else {
    uemutex.unlock();
  }
}

JSContext *
ase_js_script_engine::new_threadfree_jscontext()
{
  JSContext *cx = 0;
  {
    ase_mutex_guard<ase_recursive_mutex> g(get_uemutex());
    if (!jsctx_pool.empty()) {
      cx = jsctx_pool.back();
      jsctx_pool.pop_back();
    }
  }
  if (cx) {
    JS_SetContextThread(cx);
    begin_request(cx);
    return cx;
  }
  cx = JS_NewContext(jsruntime, 8L * 1024L);
  DBG_POOL(fprintf(stderr, "threadfree jsctx %p\n", cx));
  if (!cx) {
    handle_js_error(cx);
    return 0;
  }
  JS_SetContextPrivate(cx, this);
  JS_SetGlobalObject(cx, jglobal);
  JS_SetErrorReporter(cx, ase_js_report_error);
  JS_SetBranchCallback(cx, ase_js_branch_callback);
  begin_request(cx);
  return cx;
}

void
ase_js_script_engine::destroy_threadfree_jscontext(JSContext *cx)
{
  if (!cx) {
    return;
  }
  end_request(cx);
  JS_ClearContextThread(cx);
  {
    ase_mutex_guard<ase_recursive_mutex> g(get_uemutex());
    /* we must not throw anything */
    try {
      jsctx_pool.push_back(cx);
    } catch (...) {
      JS_DestroyContext(cx);
    }
  }
}

JSBool
ase_js_script_engine::gc_callback(JSContext *cx, JSGCStatus status)
{
  if (status == JSGC_BEGIN) {
    DBG_GC(struct mallinfo mi = ::mallinfo());
    DBG_GC(fprintf(stderr, "GCBEGIN uordblks: %d\n", mi.uordblks));
    DBG_GC(fprintf(stderr, "GCBEGIN gcbytes: %d\n", cx->runtime->gcBytes));
  } else if (status == JSGC_END) {
    DBG_GC(struct mallinfo mi = ::mallinfo());
    DBG_GC(fprintf(stderr, "GCEND uordblks: %d\n", mi.uordblks));
    DBG_GC(fprintf(stderr, "GCEND gcbytes: %d\n", cx->runtime->gcBytes));
    DBG_GCMETER(js_DumpGCStats(cx->runtime, stderr));
  }
  return JS_TRUE;
}


ase_atomic_count ase_js_script_engine::script_engine_count = 0;

}; // namespace asejs

