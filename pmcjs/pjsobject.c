
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <stdlib.h>
#include <string.h>
#include "pmcjs_impl.h"
#include "pjsobject.h"
#include "pjsobjiter.h"
#include "pmconv.h"
#include "pmutf.h"
#include "utfutil.h"

#define DBG_ROOT(x)
#define DBG_INVOKE(x)
#define DBG_INVOKE_DMP(x)
#define DBG_MEM(x)
#define DBG_CALL(x)
#define DBG_MAGIC(x)

struct pmc_jsobject_s {
  char magic[8];
  PMCPerl *ppl;
  JSObject *obj;
  pmc_listnode *node;
  pmc_jsobjiter *internal_iter;
};

static int
pmc_jsobject_is_sane(pmc_jsobject *pjo)
{
  return pjo && (memcmp(pjo->magic, "PJSOBJ", 6) == 0);
}

pmc_jsobject *
pmc_jsobject_new()
{
  pmc_jsobject *pjo;
  pjo = (pmc_jsobject *)pmc_malloc_nocx(sizeof(*pjo), "pmc_jsobject_new");
  if (!pjo) {
    return NULL;
  }
  DBG_MEM(fprintf(stderr, "__pjsm %p\n", pjo));
  memset(pjo, 0, sizeof(*pjo));
  pjo->ppl = NULL;
  pjo->obj = NULL;
  pjo->node = NULL;
  pjo->internal_iter = NULL;
  memcpy(pjo->magic, "PJSOBJ", 6);
  return pjo;
}

static void
pmc_jsobject_clear_iter(pmc_jsobject *pjo)
{
  if (pjo->internal_iter) {
    pmc_jsobjiter_destroy(pjo->internal_iter);
    pjo->internal_iter = NULL;
  }
}

static JSBool
pmc_jsobject_warn_magic(const char *mess, pmc_jsobject *pjo)
{
  fprintf(stderr, "WARNING: magic mismatch: func=[%s] ptr=[%p]\n", mess, pjo);
  DBG_MAGIC(abort());
  return JS_FALSE;
}

void
pmc_jsobject_destroy(pTHX_ pmc_jsobject *pjo)
{
  if (!pmc_jsobject_is_sane(pjo)) {
    pmc_jsobject_warn_magic("destroy", pjo);
    return;
  }
  pmc_jsobject_detach_js(aTHX_ pjo, pmc_get_current_jsctx(pjo->ppl));
  pmc_jsobject_clear_iter(pjo);
  memset(pjo, 0, sizeof(*pjo));
  pmc_free(pjo, "pmc_jsobject_destroy");
  DBG_MEM(fprintf(stderr, "__pjsf %p\n", pjo));
}

JSBool
pmc_jsobject_attach_js(pTHX_ PMCPerl *ppl, pmc_jsobject *pjo, JSContext *cx,
  JSObject *obj)
{
  pmc_listnode *nd;
  void *rp;
  if (!pmc_jsobject_is_sane(pjo)) {
    return pmc_jsobject_warn_magic("attach_js", pjo);
  }
  if (!cx || !obj) {
    return JS_TRUE;
  }
  if (!pjo) {
    return JS_TRUE;
  }
  if (pjo->ppl) {
    return JS_TRUE;
  }
  nd = pmc_register_js_proxy(ppl, cx, pjo);
  if (!nd) {
    return JS_FALSE;
  }
  rp = &pjo->obj;
  DBG_ROOT(fprintf(stderr, "addroot %p\n", rp));
  if (!JS_AddNamedRoot(cx, rp, "pmc_jsobject_attach_js")) {
    pmc_unregister_js_proxy(ppl, nd);
    return JS_FALSE;
  }
  pjo->ppl = ppl;
  pjo->obj = obj;
  pjo->node = nd;
  return JS_TRUE;
}

void
pmc_jsobject_detach_js(pTHX_ pmc_jsobject *pjo, JSContext *cx)
{
  if (!pmc_jsobject_is_sane(pjo)) {
    pmc_jsobject_warn_magic("detach_js", pjo);
    return;
  }
  if (pjo->obj) {
    PMCPerl *ppl;
    DBG_ROOT(fprintf(stderr, "removeroot %p\n", &pjo->obj));
    JS_RemoveRoot(cx, &pjo->obj);
    assert(pjo->node);
    ppl = pjo->ppl;
    if (ppl) {
      pmc_unregister_js_proxy(ppl, pjo->node);
    }
  } else {
    DBG_ROOT(fprintf(stderr, "removeroot zombie %p\n", &pjo->obj));
  }
  pjo->ppl = NULL;
  pjo->obj = NULL;
  pjo->node = NULL;
}

JSObject *
pmc_jsobject_get_js(pmc_jsobject *pjo)
{
  if (!pmc_jsobject_is_sane(pjo)) {
    pmc_jsobject_warn_magic("get_js", pjo);
    return NULL;
  }
  return pjo->obj;
}

static SV *
pmc_jsobject_invoke_internal(pTHX_ PMCPerl *ppl, JSObject *obj,
  const char *methodname, int is_utf8, int argc, SV **argv,
  const char **errstr_r, char **exception_allocated_r)
{
  JSContext *cx = NULL;
  int i = 0;
  JSObject *arrobj = NULL;
  jsval *jsargv = NULL;
  jsval elval = JSVAL_VOID;
  jsval foval = JSVAL_VOID;
  SV *retsv = NULL;
  const int binary_flag = 0;
  void *root1 = NULL, *root2 = NULL, *root3 = NULL;

  cx = pmc_get_current_jsctx(ppl);
  DBG_INVOKE(fprintf(stderr, "pmc_jsobject_invoke [%s]\n", methodname));
  (*errstr_r) = NULL;
  (*exception_allocated_r) = NULL;
  jsargv = (jsval *)pmc_malloc(cx, argc * sizeof(*jsargv),
    "pmc_jsobject_invoke");
  if (!jsargv) {
    (*errstr_r) = "JavaScript::Invoke: out of memory";
    return NULL;
  }

  if (!JS_AddNamedRoot(cx, &arrobj, "pmc_jsobject_invoke_1")) {
    (*errstr_r) = "JavaScript::Invoke: addroot failed";
    goto out;
  }
  root1 = &arrobj;
  if (!JS_AddNamedRoot(cx, &elval, "pmc_jsobject_invoke_2")) {
    (*errstr_r) = "JavaScript::Invoke: addroot failed";
    goto out;
  }
  root2 = &elval;
  if (!JS_AddNamedRoot(cx, &foval, "pmc_jsobject_invoke_3")) {
    (*errstr_r) = "JavaScript::Invoke: addroot failed";
    goto out;
  }
  root3 = &foval;

  arrobj = JS_NewArrayObject(cx, argc, NULL);
  if (!arrobj) {
    (*errstr_r) = "JavaScript::Invoke: newarray failed";
    goto out;
  }
  DBG_INVOKE(fprintf(stderr, "invoke arg begin argc=%d\n", argc));
  for (i = 0; i < argc; ++i) {
    elval = JSVAL_VOID;
    DBG_INVOKE(fprintf(stderr, " invoke arg conv vaddr=%p\n", &elval));
    if (!pmc_sv_to_jsval(aTHX_ ppl, cx, pmc_tojs_mode_auto, argv[i],
      &elval)) {
      (*errstr_r) = "JavaScript::Invoke: to_js failed";
      goto out;
    }
    if (!JS_SetElement(cx, arrobj, i, &elval)) {
      (*errstr_r) = "JavaScript::Invoke: setelement failed";
      goto out;
    }
    jsargv[i] = elval;
  }

  DBG_INVOKE(fprintf(stderr, "invoke arg end\n"));
  if (methodname) {
    /* if the method is called using the $obj->method(...) syntax,
      methodname is not utf8 enabled even if method is a utf8 symbol. to
      workaround this problem, we assume methodname is always in utf8. */
    is_utf8 = 1;
    if (!pmc_utf_getproperty(cx, obj, methodname, is_utf8, &foval)) {
      (*errstr_r) = "JavaScript::Invoke: failed to get method";
      goto out;
    }
  } else {
    /* if methodname is not specified, invoke obj itself. */
    foval = OBJECT_TO_JSVAL(obj);
  }
  DBG_INVOKE(fprintf(stderr, "method=%s foval=%p argc=%d\n", methodname,
    JSVAL_TO_OBJECT(foval), argc));
  if (!JSVAL_IS_OBJECT(foval)) {
    /* don't check whether JS_ObjectIsFunction(foval). JS_CallFunctionValue
      allows non-function objects to be invoked. */
    (*exception_allocated_r) = pmc_strcat(cx,
      methodname, " is not a function at JavaScript::Invoke",
      "pmc_jsobject_invoke_internal");
    if ((*exception_allocated_r) == NULL) {
      (*errstr_r) = "JavaScript::Invoke: out of memory";
    }
    goto out;
  }

  if (!JS_CallFunctionValue(cx, obj, foval, argc, jsargv, &elval)) {
    size_t u8len = 0;
    elval = JSVAL_VOID;
    DBG_INVOKE(fprintf(stderr, "invoke [%s] failed\n", methodname));
    JS_GetPendingException(cx, &elval);
    (*errstr_r) = "JavaScript::Invoke: js exception";
    if (elval == JSVAL_VOID) {
      (*errstr_r) = "JavaScript::Invoke: failed";
    } else {
      (*exception_allocated_r) = pmc_utf_jsval_getstring(cx, elval,
	&u8len, "JS::Invoke");
      if ((*exception_allocated_r) == NULL) {
	(*errstr_r) = "JavaScript::Invoke: out of memory";
      }
    }
    goto out;
  }
  DBG_INVOKE(fprintf(stderr, "callfunc [%s] success\n", methodname));
  DBG_INVOKE_DMP(pmc_dump_jsval(cx, "invoke retval", elval));
  retsv = pmc_jsval_to_sv(aTHX_ ppl, cx, elval, binary_flag);
  if (!retsv) {
    (*errstr_r) = "JavaScript::Invoke: to_sv failed";
  }

 out:

  if (root3) {
    JS_RemoveRoot(cx, root3);
  }
  if (root2) {
    JS_RemoveRoot(cx, root2);
  }
  if (root1) {
    JS_RemoveRoot(cx, root1);
  }
  if (jsargv) {
    pmc_free(jsargv, "pmc_jsobject_invoke");
  }
  if (!retsv) {
    JS_ClearPendingException(cx);
  }
  return retsv;
}

SV *
pmc_jsobject_invoke(pTHX_ pmc_jsobject *pjo, const char *methodname,
  int is_utf8, int argc, SV **argv, const char **errstr_r,
  char **exception_allocated_r)
{
  DBG_INVOKE(fprintf(stderr, "pmc_jsobject_invoke [%s]\n", methodname));
  if (!pmc_jsobject_is_sane(pjo)) {
    pmc_jsobject_warn_magic("invoke", pjo);
    return NULL;
  }
  if (!pjo->obj) {
    return NULL;
  }
  return pmc_jsobject_invoke_internal(aTHX_ pjo->ppl, pjo->obj,
    methodname, is_utf8, argc, argv, errstr_r, exception_allocated_r);
}

SV *
pmc_jsobject_call(pTHX_ SV *jsctx, int new_flag, int argc, SV **argv,
  const char **errstr_r, char **exception_allocated_r)
{
  pmc_jsglobal *pjg;
  PMCPerl *ppl;
  JSContext *cx;
  JSObject *obj, *pobj;
  const char *func = NULL;
  jsval pv = JSVAL_VOID;
  SV *r = NULL;

  DBG_INVOKE(fprintf(stderr, "pmc_jsobject_call\n"));
  pjg = pmc_jsglobal_fromsv(aTHX_ jsctx);
  if (!pjg) {
    return NULL;
  }
  if (argc < 1) {
    return NULL;
  }
  DBG_CALL(fprintf(stderr, "call arg0=[%s]\n", SvPV_nolen(argv[0])));
  if (argc > 0) {
    DBG_CALL(fprintf(stderr, "call arg1=[%s]\n", SvPV_nolen(argv[1])));
  }
  ppl = pmc_jsglobal_getpmcperl(pjg);
  cx = pmc_jsglobal_getcontext(pjg);
  obj = pmc_jsglobal_getobject(pjg);
  if (!JS_GetProperty(cx, obj, "Perl", &pv)) {
    JS_ClearPendingException(cx);
    return NULL;
  }
  if (!JSVAL_IS_OBJECT(pv)) {
    return NULL;
  }
  pobj = JSVAL_TO_OBJECT(pv);
  DBG_CALL(fprintf(stderr, "call cx=[%p] oj=[%p] po=[%p] argc=%d\n",
    cx, obj, pobj, argc));
  func = new_flag ? "NewJS" : "CallJS";
  DBG_CALL(fprintf(stderr, "call func=%s\n", func));
  r = pmc_jsobject_invoke_internal(aTHX_ ppl, pobj, func, 0, argc, argv,
    errstr_r, exception_allocated_r);
  DBG_CALL(fprintf(stderr, "call r=[%p]\n", r));
  return r;
}

SV *
pmc_jsobject_eval(pTHX_ SV *jsctx, const char *str, const char **errstr_r,
  char **exception_allocated_r)
{
  pmc_jsglobal *pjg;
  PMCPerl *ppl;
  JSContext *cx;
  JSObject *obj;
  pmc_u16ch_t *u16str = NULL;
  size_t u16len = 0;
  SV *retsv = NULL;
  JSScript *scr = NULL;
  jsval result = JSVAL_VOID;
  const int binary_flag = 0;

  DBG_INVOKE(fprintf(stderr, "pmc_jsobject_eval [%s]\n", str));
  pjg = pmc_jsglobal_fromsv(aTHX_ jsctx);
  if (!pjg) {
    return NULL;
  }
  ppl = pmc_jsglobal_getpmcperl(pjg);
  cx = pmc_jsglobal_getcontext(pjg);
  obj = pmc_jsglobal_getobject(pjg);
  u16str = pmc_new_utf16str_from_utf8str(cx, str, strlen(str), &u16len,
    "pmc_jsobject_eval");
  if (!u16str) {
    (*errstr_r) = "JavaScript::Eval: utf convertion failed";
    return NULL;
  }
  scr = JS_CompileUCScript(cx, obj, u16str, u16len,
    "anonymous-eval-expression", 1);
  pmc_free(u16str, "pmc_jsobject_eval");
  if (!scr) {
    (*errstr_r) = "JavaScript::Eval: compilation failed";
    JS_ClearPendingException(cx);
    return NULL;
  }
  if (JS_ExecuteScript(cx, obj, scr, &result)) {
    retsv = pmc_jsval_to_sv(aTHX_ ppl, cx, result, binary_flag);
    if (!retsv) {
      JS_ClearPendingException(cx);
      (*errstr_r) = "JavaScript::Eval: to_sv failed";
    }
  } else {
    jsval elval = JSVAL_VOID;
    JS_GetPendingException(cx, &elval);
    if (elval == JSVAL_VOID) {
      (*errstr_r) = "JavaScript::Invoke: failed";
    } else {
      size_t u8len = 0;
      (*exception_allocated_r) = pmc_utf_jsval_getstring(cx, elval,
	&u8len, "JS::Eval");
      if ((*exception_allocated_r) == NULL) {
	(*errstr_r) = "JavaScript::Eval: out of memory";
      }
    }
    JS_ClearPendingException(cx);
  }
  JS_DestroyScript(cx, scr);
  return retsv;
}

SV *
pmc_jsobject_getprop(pTHX_ pmc_jsobject *pjo, const char *propname,
  int is_utf8, const char **errstr_r)
{
  jsval val = JSVAL_VOID;
  SV *retsv = NULL;
  const int binary_flag = 0;
  JSContext *cx = NULL;

  DBG_INVOKE(fprintf(stderr, "pmc_jsobject_getprop [%s] isutf=%d\n", propname,
    is_utf8));
  (*errstr_r) = NULL;
  if (!pmc_jsobject_is_sane(pjo)) {
    pmc_jsobject_warn_magic("getprop", pjo);
    return NULL;
  }
  if (!pjo->obj) {
    return NULL;
  }
  cx = pmc_get_current_jsctx(pjo->ppl);
  if (!JS_AddNamedRoot(cx, &val, "pmc_jsobject_getprop")) {
    (*errstr_r) = "JavaScript::GetProperty: addroot failed";
    return NULL;
  }
  if (pmc_utf_getproperty(cx, pjo->obj, propname, is_utf8, &val)) {
    retsv = pmc_jsval_to_sv(aTHX_ pjo->ppl, cx, val, binary_flag);
    if (!retsv) {
      JS_ClearPendingException(cx);
      (*errstr_r) = "JavaScript::GetProperty: to_sv failed";
    }
  } else {
    (*errstr_r) = "JavaScript::GetProperty: failed to get property";
  }
  JS_RemoveRoot(cx, &val);
  return retsv;
}

SV *
pmc_jsobject_getelem(pTHX_ pmc_jsobject *pjo, int idx, const char **errstr_r)
{
  jsval val = JSVAL_VOID;
  SV *retsv = NULL;
  const int binary_flag = 0;
  JSContext *cx = NULL;

  DBG_INVOKE(fprintf(stderr, "pmc_jsobject_getelem %d\n", idx));
  (*errstr_r) = NULL;
  if (!pmc_jsobject_is_sane(pjo)) {
    pmc_jsobject_warn_magic("getelem", pjo);
    return NULL;
  }
  if (!pjo->obj) {
    return NULL;
  }
  cx = pmc_get_current_jsctx(pjo->ppl);
  if (!JS_AddNamedRoot(cx, &val, "pmc_jsobject_getelem")) {
    (*errstr_r) = "JavaScript::GetElement: addroot failed";
    JS_ClearPendingException(cx);
    return NULL;
  }
  if (JS_GetElement(cx, pjo->obj, idx, &val)) {
    retsv = pmc_jsval_to_sv(aTHX_ pjo->ppl, cx, val, binary_flag);
    if (!retsv) {
      JS_ClearPendingException(cx);
      (*errstr_r) = "JavaScript::GetElement: to_sv failed";
    }
  } else {
    (*errstr_r) = "JavaScript::GetElement failed";
  }
  JS_RemoveRoot(cx, &val);
  return retsv;
}

const char *
pmc_jsobject_setprop(pTHX_ pmc_jsobject *pjo, const char *propname,
  int is_utf8, SV *propval)
{
  jsval val = JSVAL_VOID;
  const char *errstr = NULL;
  JSContext *cx = NULL;

  DBG_INVOKE(fprintf(stderr, "pmc_jsobject_setprop [%s] isutf=%d\n", propname,
    is_utf8));
  if (!pmc_jsobject_is_sane(pjo)) {
    pmc_jsobject_warn_magic("setprop", pjo);
    return NULL;
  }
  if (!pjo->obj) {
    return NULL;
  }
  cx = pmc_get_current_jsctx(pjo->ppl);
  if (!JS_AddNamedRoot(cx, &val, "pmc_jsobject_setprop")) {
    return "JavaScript::SetProperty: addroot failed";
  }
  if (!pmc_sv_to_jsval(aTHX_ pjo->ppl, cx, pmc_tojs_mode_auto, propval,
    &val)) {
    errstr = "JavaScript::SetProperty: to_js failed";
    JS_ClearPendingException(cx);
  } else if (pmc_utf_setproperty(cx, pjo->obj, propname, is_utf8,
    &val)) {
    errstr = NULL;
  } else {
    errstr = "JavaScript::SetProperty failed";
    JS_ClearPendingException(cx);
  }
  JS_RemoveRoot(cx, &val);
  return errstr;
}

const char *
pmc_jsobject_setelem(pTHX_ pmc_jsobject *pjo, int idx, SV *propval)
{
  jsval val = JSVAL_VOID;
  const char *errstr = NULL;
  JSContext *cx = NULL;

  DBG_INVOKE(fprintf(stderr, "pmc_jsobject_setelem %d\n", idx));
  if (!pmc_jsobject_is_sane(pjo)) {
    pmc_jsobject_warn_magic("setelem", pjo);
    return NULL;
  }
  if (!pjo->obj) {
    return NULL;
  }
  cx = pmc_get_current_jsctx(pjo->ppl);
  if (!JS_AddNamedRoot(cx, &val, "pmc_jsobject_setelem")) {
    return "JavaScript::SetElement: addroot failed";
  }
  if (!pmc_sv_to_jsval(aTHX_ pjo->ppl, cx, pmc_tojs_mode_auto, propval,
    &val)) {
    errstr = "JavaScript::SetElement: to_js failed";
    JS_ClearPendingException(cx);
  } else if (JS_SetElement(cx, pjo->obj, idx, &val)) {
    errstr = NULL;
  } else {
    errstr = "JavaScript::SetElement failed";
    JS_ClearPendingException(cx);
  }
  JS_RemoveRoot(cx, &val);
  return errstr;
}

const char *
pmc_jsobject_delprop(pTHX_ pmc_jsobject *pjo, const char *propname,
  int is_utf8)
{
  const char *errstr = NULL;
  jsval val = JSVAL_VOID;
  JSContext *cx = NULL;

  DBG_INVOKE(fprintf(stderr, "pmc_jsobject_delprop [%s] isutf=%d\n", propname,
    is_utf8));
  if (!pmc_jsobject_is_sane(pjo)) {
    pmc_jsobject_warn_magic("delprop", pjo);
    return NULL;
  }
  if (!pjo->obj) {
    return NULL;
  }
  cx = pmc_get_current_jsctx(pjo->ppl);
  if (!JS_AddNamedRoot(cx, &val, "pmc_jsobject_delprop")) {
    return "JavaScript::DelProperty: addroot failed";
  }
  if (pmc_utf_delproperty(cx, pjo->obj, propname, is_utf8, &val)) {
    errstr = NULL;
  } else {
    errstr = "JavaScript::DelProperty failed";
    JS_ClearPendingException(cx);
  }
  JS_RemoveRoot(cx, &val);
  return errstr;
}

const char *
pmc_jsobject_delelem(pTHX_ pmc_jsobject *pjo, int idx)
{
  const char *errstr = NULL;
  JSContext *cx = NULL;

  DBG_INVOKE(fprintf(stderr, "pmc_jsobject_delelem %d\n", idx));
  if (!pmc_jsobject_is_sane(pjo)) {
    pmc_jsobject_warn_magic("delelem", pjo);
    return NULL;
  }
  if (!pjo->obj) {
    return NULL;
  }
  cx = pmc_get_current_jsctx(pjo->ppl);
  if (JS_DeleteElement(cx, pjo->obj, idx)) {
    errstr = NULL;
  } else {
    errstr = "JavaScript::DelElement failed";
    JS_ClearPendingException(cx);
  }
  return errstr;
}

SV *
pmc_jsobject_getlength(pTHX_ pmc_jsobject *pjo)
{
  SV *retsv = NULL;
  jsuint len = 0;
  JSContext *cx = NULL;

  DBG_INVOKE(fprintf(stderr, "pmc_jsobject_getlength\n"));
  if (!pmc_jsobject_is_sane(pjo)) {
    pmc_jsobject_warn_magic("getlength", pjo);
    return NULL;
  }
  if (!pjo->obj) {
    return NULL;
  }
  cx = pmc_get_current_jsctx(pjo->ppl);
  if (JS_GetArrayLength(cx, pjo->obj, &len)) {
    retsv = newSViv(len);
  }
  return retsv;
}

const char *
pmc_jsobject_setlength(pTHX_ pmc_jsobject *pjo, int len)
{
  const char *errstr = NULL;
  JSContext *cx = NULL;

  DBG_INVOKE(fprintf(stderr, "pmc_jsobject_setlength\n"));
  if (!pmc_jsobject_is_sane(pjo)) {
    pmc_jsobject_warn_magic("setlength", pjo);
    return NULL;
  }
  if (!pjo->obj) {
    return NULL;
  }
  cx = pmc_get_current_jsctx(pjo->ppl);
  if (JS_SetArrayLength(cx, pjo->obj, (jsuint)len)) {
    errstr = NULL;
  } else {
    errstr = "JavaScript::SetLength failed";
    JS_ClearPendingException(cx);
  }
  return errstr;
}

const char *
pmc_jsobject_resetiterator(pTHX_ pmc_jsobject *pjo)
{
  JSContext *cx = NULL;

  DBG_INVOKE(fprintf(stderr, "pmc_jsobject_resetiterator\n"));
  pmc_jsobject_clear_iter(pjo);
  if (!pmc_jsobject_is_sane(pjo)) {
    pmc_jsobject_warn_magic("resetiterator", pjo);
    return NULL;
  }
  if (!pjo->obj) {
    return NULL;
  }
  cx = pmc_get_current_jsctx(pjo->ppl);
  pjo->internal_iter = pmc_jsobjiter_new(cx, pjo->obj);
  if (pjo->internal_iter == NULL) {
    return "JavaScript::ResetIterator: out of memory";
  }
  return NULL;
}

SV *
pmc_jsobject_iterate(pTHX_ pmc_jsobject *pjo)
{
  const char *key = NULL;

  DBG_INVOKE(fprintf(stderr, "pmc_jsobject_iterate\n"));
  if (!pmc_jsobject_is_sane(pjo)) {
    pmc_jsobject_warn_magic("iterate", pjo);
    return NULL;
  }
  if (!pjo->obj) {
    return NULL;
  }
  if (pjo->internal_iter == NULL) {
    return NULL;
  }
  key = pmc_jsobjiter_next(pjo->internal_iter);
  if (!key) {
    pmc_jsobject_clear_iter(pjo);
    return NULL;
  }
  return newSVpv(key, 0);
}

