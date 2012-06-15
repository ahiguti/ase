
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include "pmcexception.h"
#include "utfutil.h"
#include "polist.h"
#include "pmutf.h"

#define DBG(x)
#define DBG_FIN(x)

static pmc_list *
pmc_exc_strlist_new(const char *debugid)
{
  return pmc_list_new(debugid);
}

static void
pmc_exc_strlist_destroy(pmc_list *lst, const char *debugid)
{
  pmc_listnode *nd;
  for (nd = pmc_list_first(lst); nd; nd = pmc_list_next(lst, nd)) {
    char *s;
    s = (char *)pmc_listnode_get_value(nd);
    pmc_free(pmc_listnode_get_value(nd), "pmc_exc_strlist_destroy");
  }
  pmc_list_destroy(lst, debugid);
}

static pmc_listnode *
pmc_exc_strlist_append_str(pmc_list *lst, char *str, const char *debugid)
  /* str will be owned by lst */
{
  pmc_listnode *nd;
  if (!str) {
    return NULL;
  }
  nd = pmc_list_insert_before(lst, NULL, str);
  if (!nd) {
    pmc_free(str, debugid);
  }
  return nd;
}

static pmc_listnode *
pmc_exc_strlist_append_literal(JSContext *cx, pmc_list *lst, char *str,
  const char *debugid)
{
  char *s;
  s = pmc_strdup(cx, str, debugid);
  if (!s) {
    return NULL;
  }
  return pmc_exc_strlist_append_str(lst, s, debugid);
}

static char *
pmc_exc_strlist_join(JSContext *cx, pmc_list *lst, const char *debugid)
{
  char *s;
  pmc_listnode *nd;
  size_t len = 0, i = 0;
  for (nd = pmc_list_first(lst); nd; nd = pmc_list_next(lst, nd)) {
    len += (size_t)strlen((char *)pmc_listnode_get_value(nd));
  }
  s = (char *)pmc_malloc(cx, len + 1, debugid);
  if (!s) {
    return NULL;
  }
  for (nd = pmc_list_first(lst); nd; nd = pmc_list_next(lst, nd)) {
    char *ns;
    size_t l = 0;
    ns = (char *)pmc_listnode_get_value(nd);
    l = strlen(ns);
    memcpy(s + i, ns, l);
    i += l;
  }
  assert(i == len);
  s[i] = '\0';
  return s;
}

static pmc_listnode *
pmc_exc_strlist_append_prop(pmc_list *lst, JSContext *cx, JSObject *obj,
  const char *propname, const char *debugid)
{
  char *s;
  jsval jv = JSVAL_VOID;
  size_t len = 0;
  if (!JS_GetProperty(cx, obj, propname, &jv)) {
    jv = JSVAL_VOID;
  }
  s = pmc_utf_jsval_getstring(cx, jv, &len, debugid);
  if (!s) {
    return NULL;
  }
  return pmc_exc_strlist_append_str(lst, s, debugid);
}

static pmc_listnode *
pmc_exc_strlist_append_trace(pmc_list *lst, JSContext *cx, JSObject *obj,
  const char *propname, const char *debugid)
{
  jsval jv = JSVAL_VOID;
  if (!JS_GetProperty(cx, obj, propname, &jv)) {
    jv = JSVAL_VOID;
  }
  if (JSVAL_IS_OBJECT(jv) && JS_IsArrayObject(cx, JSVAL_TO_OBJECT(jv))) {
    JSObject *arr;
    jsuint len = 0, i = 0;
    arr = JSVAL_TO_OBJECT(jv);
    if (!JS_GetArrayLength(cx, arr, &len)) {
      len = 0;
    }
    for (i = 0; i < len; ++i) {
      jsval ev = JSVAL_VOID;
      if (JS_GetElement(cx, arr, i, &ev) && JSVAL_IS_OBJECT(ev)) {
	JSObject *tobj;
	tobj = JSVAL_TO_OBJECT(ev);
	if (!pmc_exc_strlist_append_literal(cx, lst, "\n  func=[", debugid) ||
	    !pmc_exc_strlist_append_prop(lst, cx, tobj, "func", debugid) ||
	    !pmc_exc_strlist_append_literal(cx, lst, "] file=", debugid) || 
	    !pmc_exc_strlist_append_prop(lst, cx, tobj, "file", debugid) ||
	    !pmc_exc_strlist_append_literal(cx, lst, " line=", debugid) || 
	    !pmc_exc_strlist_append_prop(lst, cx, tobj, "line", debugid)) {
	  return NULL;
	}
      }
    }
  }
  return pmc_list_last(lst);
}

static JSBool
pmc_exc_tostring_internal(JSContext *cx, JSObject *obj, jsval *rval)
{
  JSBool suc = JS_TRUE;
  pmc_list *lst = NULL;
  static const char *debugid = "pmc_exc_tostring";
  char *str = NULL;
  lst = pmc_exc_strlist_new("pmc_exc_tostring");
  if (!lst) {
    goto err;
  }
  if (
    !pmc_exc_strlist_append_literal(cx, lst, "(\n etype: ", debugid) ||
    !pmc_exc_strlist_append_prop(lst, cx, obj, "etype", debugid) ||
    !pmc_exc_strlist_append_literal(cx, lst, ",\n error: ", debugid) ||
    !pmc_exc_strlist_append_prop(lst, cx, obj, "error", debugid) ||
    !pmc_exc_strlist_append_literal(cx, lst, ",\n func: ", debugid) ||
    !pmc_exc_strlist_append_prop(lst, cx, obj, "func", debugid) ||
    !pmc_exc_strlist_append_literal(cx, lst, ",\n description: ", debugid) ||
    !pmc_exc_strlist_append_prop(lst, cx, obj, "description", debugid) ||
    !pmc_exc_strlist_append_literal(cx, lst, ",\n at: ", debugid) ||
    !pmc_exc_strlist_append_trace(lst, cx, obj, "at", debugid) ||
    !pmc_exc_strlist_append_literal(cx, lst, "\n)", debugid)) {
    goto err;
  }
  str = pmc_exc_strlist_join(cx, lst, debugid);
  if (!str) {
    goto err;
  }
  if (!pmc_utf_jsval_setstring(cx, str, strlen(str), rval)) {
    goto err;
  }
  goto out;

 err:

  suc = JS_FALSE;
  (*rval) = JSVAL_VOID;

 out:

  if (lst) {
    pmc_exc_strlist_destroy(lst, "pmc_exc_tostring");
  }
  if (str) {
    pmc_free(str, "pmc_exc_tostring");
  }
  return suc;
}

static JSBool
pmc_exc_tostring(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
  jsval *rval)
{
  return pmc_exc_tostring_internal(cx, obj, rval);
}

static void
pmc_exc_finalize(JSContext *cx, JSObject *obj)
{
  DBG_FIN(fprintf(stderr, "pmc_exc_finalize %p\n", obj));
}

static JSFunctionSpec pmc_exception_methods[] = {
  { "toString", pmc_exc_tostring, 0, 0, 0 },
  { NULL, NULL, 0, 0, 0 },
};

JSClass pmc_exception_class = {
  "PMCException", 0,
  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, pmc_exc_finalize,
};

JSObject *
pmc_init_pmc_exception_class(JSContext *cx, JSObject *obj)
{
  JSObject *proto;
  DBG(fprintf(stderr, "InitPMCExceptionClass\n"));
  proto = JS_InitClass(cx, obj, NULL, &pmc_exception_class, NULL,
    0, NULL, pmc_exception_methods, NULL, NULL);
  return proto;
}

