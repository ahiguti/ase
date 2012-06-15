
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/aseutf16.hpp>

#include <cassert>

#include "asejs_sctx.hpp"
#include "asejs_asepr.hpp"
#include "asejs_util.hpp"

#ifdef _MSC_VER
#include <malloc.h>
#define ASE_JS_STACK_ALLOCA(x) _alloca(x)
#else
#include <alloca.h>
#define ASE_JS_STACK_ALLOCA(x) alloca(x)
#endif

#define DBG_CONVERT(x)
#define DBG_GETPROP(x)
#define DBG_LOCK(x)
#define DBG_ARR(x)
#define DBG_CALLRET(x)
#define DBG_CALL(x)
#define DBG_MCALL(x)
#define DBG_TOSTR(x)

namespace asejs {

class callfromjs_guard {

 public:

  callfromjs_guard(const ase_variant& v, ase_js_script_engine *sc,
    JSContext *cx, const char *mess)
    : unlock_flag(
      (v.get_vtoption() & ase_vtoption_multithreaded) != 0),
      script_engine(sc), jscontext(cx), saved_depth(0), dbgmess(mess) {
    if (unlock_flag) {
      saved_depth = script_engine->suspend_request(jscontext);
      DBG_LOCK(fprintf(stderr, "callfromjs constr unlock %p %s %lu\n",
	&script_engine->get_mutex(), dbgmess, pthread_self()));
    }
  }
  ~callfromjs_guard() /* DCHK */ {
    if (unlock_flag) {
      DBG_LOCK(fprintf(stderr, "callfromjs destr lock %p %s %lu\n",
	&script_engine->get_mutex(), dbgmess, pthread_self()));
      script_engine->resume_request(jscontext, saved_depth);
    }
  }

 private:

  bool unlock_flag;
  ase_js_script_engine *const script_engine;
  JSContext *const jscontext;
  jsrefcount saved_depth;
  const char *const dbgmess;

};

ase_js_aseproxy_privnode *
ase_js_aseproxy_get_private(JSContext *cx, JSObject *obj)
{
  if (JS_GET_CLASS(cx, obj) != &ase_js_aseproxy_class) {
    return 0;
  }
  void *p = JS_GetPrivate(cx, obj);
  if (!p) {
    return 0;
  }
  return static_cast<ase_js_aseproxy_privnode *>(p);
}

JSBool
ase_js_aseproxy_getprop(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
  DBG_GETPROP(std::cerr << "getprop begin" << std::endl);
  ase_js_aseproxy_privnode *priv = ase_js_aseproxy_get_private(cx, obj);
  if (!priv) {
    return JS_TRUE;
  }
  ase_js_script_engine *const sc = priv->value.get_script_engine();
  try {
    if (JSVAL_IS_INT(id)) {
      int idx = JSVAL_TO_INT(id);
      ase_variant av;
      {
	callfromjs_guard g(priv->value, sc, cx, "getpropint");
	av = priv->value.get_element(idx);
      }
      ase_js_scoped_root rv = ase_js_util::to_js_value_no_root(
	sc, cx, av);
      (*vp) = rv.get();
    } else {
      ase_string name8 = ase_js_util::get_string(sc, cx, id);
      DBG_GETPROP(std::cerr << "getprop " << name8 << std::endl);
      /* attributes? */
      if (name8 == "__aseattr__") {
	ase_int atr = priv->value.get_attributes_internal();
	(*vp) = INT_TO_JSVAL(atr);
	return JS_TRUE;
      }
      ase_variant av;
      {
	callfromjs_guard g(priv->value, sc, cx, "getpropstr");
	av = priv->value.get_property(name8.data(), name8.size());
      }
      ase_js_scoped_root rv = ase_js_util::to_js_value_no_root(sc, cx, av);
      (*vp) = rv.get();
      DBG_GETPROP(fprintf(stderr, "getprop retobj=%p %s\n", (void *)(*vp),
	name8.data()));
    }
    return JS_TRUE;
  } catch (...) {
    ase_js_util::handle_exception(sc, cx);
    return JS_FALSE;
  }
}

JSBool
ase_js_aseproxy_setprop(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
  ase_js_aseproxy_privnode *priv = ase_js_aseproxy_get_private(cx, obj);
  if (!priv) {
    return JS_TRUE;
  }
  ase_js_script_engine *const sc = priv->value.get_script_engine();
  try {
    ase_variant val = ase_js_util::to_ase_variant(sc, cx, (*vp));
    if (JSVAL_IS_INT(id)) {
      int idx = JSVAL_TO_INT(id);
      {
	callfromjs_guard g(priv->value, sc, cx, "setpropint");
	priv->value.set_element(idx, val);
      }
    } else {
      ase_string name8 = ase_js_util::get_string(sc, cx, id);
      {
	callfromjs_guard g(priv->value, sc, cx, "setpropstr");
	priv->value.set_property(name8.data(), name8.size(), val);
      }
    }
    return JS_TRUE;
  } catch (...) {
    ase_js_util::handle_exception(sc, cx);
    return JS_FALSE;
  }
}

JSBool
ase_js_aseproxy_delprop(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
  ase_js_aseproxy_privnode *priv = ase_js_aseproxy_get_private(cx, obj);
  if (!priv) {
    return JS_TRUE;
  }
  ase_js_script_engine *const sc = priv->value.get_script_engine();
  try {
    ase_string name8 = ase_js_util::get_string(sc, cx, id);
    {
      callfromjs_guard g(priv->value, sc, cx, "delprop");
      priv->value.del_property(name8.data(), name8.size());
    }
    return JS_TRUE;
  } catch (...) {
    ase_js_util::handle_exception(sc, cx);
    return JS_FALSE;
  }
}

JSBool
ase_js_aseproxy_enumerate(JSContext *cx, JSObject *obj)
{
  ase_js_aseproxy_privnode *priv = ase_js_aseproxy_get_private(cx, obj);
  if (!priv) {
    return JS_TRUE;
  }
  ase_js_script_engine *const sc = priv->value.get_script_engine();
  try {
    JS_ClearScope(cx, obj);
    ase_variant ev;
    {
      callfromjs_guard g(priv->value, sc, cx, "getenum");
      ev = priv->value.get_enumerator();
    }
    while (true) {
      ase_string key;
      bool hasnext = false;
      {
	callfromjs_guard g(ev, sc, cx, "enumnext");
	key = ev.enum_next(hasnext);
      }
      if (!hasnext) {
	break;
      }
      ase_utf16_string u16str(key);
      if (!JS_DefineUCProperty(cx, obj, ase_js_util::to_jschar(u16str.data()),
        u16str.size(), JSVAL_VOID, 0, 0, JSPROP_ENUMERATE)) {
	sc->handle_js_error(cx);
	return JS_FALSE;
      }
    }
    return JS_TRUE;
  } catch (...) {
    ase_js_util::handle_exception(sc, cx);
    return JS_FALSE;
  }
}

JSBool
ase_js_aseproxy_convert(JSContext *cx, JSObject *obj, JSType ty, jsval *val_r)
{
  DBG_CONVERT(fprintf(stderr, "CONVERT obj=%p ty=%d\n", obj, ty));
  if (ty == 3) {
    return ase_js_aseproxy_to_string(cx, obj, 0, 0, val_r);
  }
  return JS_TRUE;
}

JSBool
ase_js_aseproxy_constr(JSContext *cx, JSObject *obj, uintN argc,
  jsval *argv, jsval *val_r)
{
  (*val_r) = JSVAL_VOID;
  return JS_TRUE;
}

void
ase_js_aseproxy_finalize(JSContext *cx, JSObject *obj)
{
  ase_js_aseproxy_privnode *priv = ase_js_aseproxy_get_private(cx, obj);
  if (!priv) {
    return;
  }
  /* must not unlock script_engine here because we're GCing now. note that
    script_engine must have recursive lock because priv->release() may
    call js code again. */
  delete priv;
}

struct ase_js_asearrref {
  ase_js_asearrref() : arr(0), num(0) { }
  ~ase_js_asearrref() /* DCHK */ {
    for (uintN i = 0; i < num; ++i) {
      DBG_ARR(fprintf(stderr, "asearrref destr %u %p\n", i,
	arr[i].get_rep().p));
      arr[i].~ase_variant(); /* DCHK */
    }
  }
  void set(ase_variant *a, uintN n, jsval *argv, ase_js_script_engine *sc,
    JSContext *cx) __attribute__((nonnull(1))) {
    assert(arr == 0);
    arr = a;
    for (uintN i = 0; i < n; ++i) {
      new(a + i) ase_variant(ase_js_util::to_ase_variant(sc, cx, argv[i]));
      DBG_ARR(fprintf(stderr, "asearrref constr %u %p\n", i,
	a[i].get_rep().p));
      num = i + 1;
    }
  }
  ase_variant *arr;
  uintN num;
};

JSBool
ase_js_aseproxy_call_internal(JSContext *cx, JSObject *obj, uintN argc,
  jsval *argv, jsval *val_r)
{
  if (argc > 64) {
    /* limit argc for alloca safety */
    (*val_r) = JSVAL_VOID;
    return JS_TRUE;
  }
  DBG_CALL(fprintf(stderr, "call obj=%p\n", obj));
  ase_js_aseproxy_privnode *priv = ase_js_aseproxy_get_private(cx, obj);
  if (!priv) {
    return JS_TRUE;
  }
  ase_js_aseproxy_priv& value = priv->value;
  ase_js_script_engine *const sc = value.get_script_engine();
  try {
    ase_variant *aargs = static_cast<ase_variant *>(
      ASE_JS_STACK_ALLOCA((argc + 1) * sizeof(ase_variant)));
	/* +1 to avoid alloca(0) */
    ase_js_asearrref arrref;
    arrref.set(aargs, argc, argv, sc, cx);
    ase_variant av;
    {
      callfromjs_guard g(value, sc, cx, "call");
      av = value.invoke_self(aargs, argc);
    }
    DBG_CALLRET(fprintf(stderr, "js asepr invoke retval=%p\n",
      av.get_rep().p));
    ase_js_scoped_root rv = ase_js_util::to_js_value_no_root(sc, cx, av);
    (*val_r) = rv.get();
    return JS_TRUE;
  } catch (...) {
    ase_js_util::handle_exception(sc, cx);
    return JS_FALSE;
  }
}

JSBool
ase_js_aseproxy_call(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
  jsval *val_r)
{
  if (!JSVAL_IS_OBJECT(argv[-2])) {
    (*val_r) = JSVAL_VOID;
    return JS_TRUE;
  }
  obj = jsval_to_object(argv[-2]); /* overwrite obj */
  return ase_js_aseproxy_call_internal(cx, obj, argc, argv, val_r);
}

JSBool
ase_js_aseproxy_to_string(JSContext *cx, JSObject *obj, uintN argc,
  jsval *argv, jsval *val_r)
{
  DBG_TOSTR(fprintf(stderr, "ASEPR TOSTRING\n"));
  ase_js_aseproxy_privnode *priv = ase_js_aseproxy_get_private(cx, obj);
  if (!priv) {
    return JS_TRUE;
  }
  ase_js_script_engine *const sc = priv->value.get_script_engine();
  try {
    ase_string s;
    {
      callfromjs_guard g(priv->value, sc, cx, "getstring");
      s = priv->value.get_string();
    }
    DBG_TOSTR(fprintf(stderr, "JS TOSTRING GETSTRING %d\n",
      priv->value.get_type()));
    DBG_TOSTR(fprintf(stderr, "JS TOSTRING GETSTRING %s\n",
      s.data()));
    ase_variant av = s.to_variant();
    ase_js_scoped_root rv = ase_js_util::to_js_value_no_root(sc, cx, av);
    (*val_r) = rv.get();
    return JS_TRUE;
  } catch (...) {
    ase_js_util::handle_exception(sc, cx);
    return JS_FALSE;
  }
}

JSBool
ase_js_asemethodproxy_call(JSContext *cx, JSObject *obj, uintN argc,
  jsval *argv, jsval *val_r)
{
  if (!JSVAL_IS_OBJECT(argv[-2]) || argc > 64) {
    /* limit argc for alloca safety */
    (*val_r) = JSVAL_VOID;
    return JS_TRUE;
  }
  JSObject *fobj = jsval_to_object(argv[-2]);
  if (JS_GET_CLASS(cx, fobj) != &ase_js_asemethodproxy_class) {
    DBG_MCALL(fprintf(stderr, "mcall fobj is not mpr\n"));
    (*val_r) = JSVAL_VOID;
    return JS_TRUE;
  }
  void *protopriv = JS_GetPrivate(cx, fobj);
  DBG_MCALL(fprintf(stderr, "mcall mpobj=%p priv=%p\n", fobj, protopriv));
  const ase_size_type id = reinterpret_cast<ase_size_type>(protopriv) / 8;
  DBG_MCALL(fprintf(stderr, "mcall id=%lu\n", (unsigned long)id));
  DBG_MCALL(fprintf(stderr, "mcall obj=%p\n", obj));
  DBG_MCALL(fprintf(stderr, "mcall fobjp=%p\n", obj));
  if (JS_GET_CLASS(cx, obj) != &ase_js_aseproxy_class) {
    DBG_MCALL(fprintf(stderr, "mcall obj is not pr %s\n",
      JS_GET_CLASS(cx, obj)->name));
    (*val_r) = JSVAL_VOID;
  }
  ase_js_aseproxy_privnode *priv = ase_js_aseproxy_get_private(cx, obj);
  if (!priv) {
    return JS_TRUE;
  }
  ase_js_aseproxy_priv& value = priv->value;
  ase_js_script_engine *const sc = value.get_script_engine();
  try {
    ase_variant *aargs = static_cast<ase_variant *>(
      ASE_JS_STACK_ALLOCA((argc + 1) * sizeof(ase_variant)));
	/* +1 to avoid alloca(0) */
    ase_js_asearrref arrref;
    arrref.set(aargs, argc, argv, sc, cx);
    ase_variant r;
    {
      callfromjs_guard g(value, sc, cx, "mcall");
      r = value.invoke_by_id(static_cast<ase_int>(id), aargs, argc);
    }
    ase_js_scoped_root rv = ase_js_util::to_js_value_no_root(sc, cx, r);
    (*val_r) = rv.get();
    DBG_CALLRET(fprintf(stderr, "mcall invoke retval=%p\n", avp->get_rep().p));
    return JS_TRUE;
  } catch (...) {
    ase_js_util::handle_exception(sc, cx);
    return JS_FALSE;
  }
}

JSBool
ase_js_asemethodproxy_convert(JSContext *cx, JSObject *obj, JSType ty,
  jsval *val_r)
{
  DBG_CONVERT(fprintf(stderr, "MCONVERT obj=%p ty=%d\n", obj, ty));
  return JS_TRUE;
}

}; // namespace asejs

