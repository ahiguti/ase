
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include "pmutf.h"
#include "utfutil.h"

#define DBG(x)
#define DBG_7BIT(x)

JSBool
pmc_utf_defineproperty(JSContext *cx, JSObject *obj, const char *name,
  size_t namelen, int is_utf8, jsval value, JSPropertyOp getter,
  JSPropertyOp setter, uintN attrs)
{
  JSBool suc = JS_TRUE;
  if (is_utf8) {
    namelen = strlen(name);
    if (pmc_utf8_is_7bit_string(name, namelen)) {
      is_utf8 = 0;
    }
  }
  if (is_utf8) {
    pmc_u16ch_t *u16str = NULL;
    size_t u16len = 0;
    u16str = pmc_new_utf16str_from_utf8str(cx, name, namelen, &u16len,
      "pmc_utf_defineproperty");
    if (!u16str) {
      return JS_FALSE;
    }
    suc = JS_DefineUCProperty(cx, obj, u16str, u16len, value, getter, setter,
      attrs);
    pmc_free(u16str, "pmc_utf_defineproperty");
  } else {
    suc = JS_DefineProperty(cx, obj, name, value, getter, setter, attrs);
  }
  return suc;
}

typedef enum {
  pmc_getsetdel_get,
  pmc_getsetdel_set,
  pmc_getsetdel_del,
} pmc_getsetdel_t;

static JSBool
pmc_utf_pmc_getsetdel_property(JSContext *cx, JSObject *obj, const char *name,
  int is_utf8, pmc_getsetdel_t gsd, jsval *vp)
{
  JSBool suc = JS_TRUE;
  int namelen = 0;
  if (is_utf8) {
    namelen = strlen(name);
    if (pmc_utf8_is_7bit_string(name, namelen)) {
      is_utf8 = 0;
    }
  }
  if (is_utf8) {
    pmc_u16ch_t *u16str = NULL;
    size_t u16len = 0;
    DBG(fprintf(stderr, "pmc_getsetdel_prop: utf8 [%s]\n", name));
    u16str = pmc_new_utf16str_from_utf8str(cx, name, namelen, &u16len,
      "pmc_utf_pmc_getsetdel_property");
    if (!u16str) {
      return JS_FALSE;
    }
    switch (gsd) {
    case pmc_getsetdel_get:
      suc = JS_GetUCProperty(cx, obj, u16str, u16len, vp);
      break;
    case pmc_getsetdel_set:
      suc = JS_SetUCProperty(cx, obj, u16str, u16len, vp);
      break;
    case pmc_getsetdel_del:
      suc = JS_DeleteUCProperty2(cx, obj, u16str, u16len, vp);
      break;
    }
    pmc_free(u16str, "pmc_utf_pmc_getsetdel_property");
  } else {
    DBG(fprintf(stderr, "pmc_getsetdel_prop: noutf8 [%s]\n", name));
    switch (gsd) {
    case pmc_getsetdel_get:
      suc = JS_GetProperty(cx, obj, name, vp);
      break;
    case pmc_getsetdel_set:
      suc = JS_SetProperty(cx, obj, name, vp);
      break;
    case pmc_getsetdel_del:
      suc = JS_DeleteProperty2(cx, obj, name, vp);
      break;
    }
  }
  return suc;
}

JSBool
pmc_utf_getproperty(JSContext *cx, JSObject *obj, const char *name,
  int is_utf8, jsval *vp)
{
  return pmc_utf_pmc_getsetdel_property(cx, obj, name, is_utf8,
    pmc_getsetdel_get, vp);
}

JSBool
pmc_utf_setproperty(JSContext *cx, JSObject *obj, const char *name,
  int is_utf8, jsval *vp)
{
  return pmc_utf_pmc_getsetdel_property(cx, obj, name, is_utf8,
    pmc_getsetdel_set, vp);
}

JSBool
pmc_utf_delproperty(JSContext *cx, JSObject *obj, const char *name,
  int is_utf8, jsval *vp)
{
  return pmc_utf_pmc_getsetdel_property(cx, obj, name, is_utf8,
    pmc_getsetdel_del, vp);
}

char *
pmc_utf_jsval_getstring(JSContext *cx, jsval val, size_t *u8len_r,
  const char *debugid)
{
  JSString *str;
  pmc_u16ch_t *u16str;
  char *u8str;
  size_t u16len = 0, u8len = 0;
  (*u8len_r) = 0;
  str = JS_ValueToString(cx, val);
  if (!str) {
    return NULL;
  }
  u16str = JS_GetStringChars(str);
  u16len = JS_GetStringLength(str);
  u8str = pmc_new_utf8str_from_utf16str(cx, u16str, u16len, &u8len,
    debugid);
  if (u8str) {
    (*u8len_r) = u8len;
  }
  return u8str;
}

JSBool
pmc_utf_jsval_setstring(JSContext *cx, const char *str, size_t slen,
  jsval *rval)
{
  JSBool suc = JS_TRUE;
  if (pmc_utf8_is_7bit_string(str, slen)) {
    JSString *jsstr = NULL;
    DBG_7BIT(fprintf(stderr, "7bit\n"));
    jsstr = JS_NewStringCopyN(cx, str, slen);
    if (jsstr) {
      (*rval) = STRING_TO_JSVAL(jsstr);
      suc = JS_TRUE;
    } else {
      (*rval) = JSVAL_VOID;
      suc = JS_FALSE;
    }
  } else {
    pmc_u16ch_t *u16str = NULL;
    size_t u16len = 0;
    JSString *jsstr = NULL;
    u16str = pmc_new_utf16str_from_utf8str(cx, str, slen, &u16len,
      "pmc_utf_jsval_setstring");
    if (!u16str) {
      (*rval) = JSVAL_VOID;
      return JS_FALSE;
    }
    jsstr = JS_NewUCStringCopyN(cx, u16str, u16len);
    if (jsstr) {
      (*rval) = STRING_TO_JSVAL(jsstr);
      suc = JS_TRUE;
    } else {
      (*rval) = JSVAL_VOID;
      suc = JS_FALSE;
    }
    pmc_free(u16str, "pmc_utf_jsval_setstring");
  }
  return suc;
}

