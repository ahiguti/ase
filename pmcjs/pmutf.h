
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef PMCJS_PMUTF_HPP
#define PMCJS_PMUTF_HPP

#include "pmcjs_impl.h"

#ifdef __cplusplus
extern "C" {
#endif

JSBool pmc_utf_getproperty(JSContext *cx, JSObject *obj, const char *name,
  int is_utf8, jsval *vp);
JSBool pmc_utf_setproperty(JSContext *cx, JSObject *obj, const char *name,
  int is_utf8, jsval *vp);
JSBool pmc_utf_delproperty(JSContext *cx, JSObject *obj, const char *name,
  int is_utf8, jsval *vp);
JSBool pmc_utf_defineproperty(JSContext *cx, JSObject *obj, const char *name,
  size_t namelen, int is_utf8, jsval value, JSPropertyOp getter,
  JSPropertyOp setter, uintN attrs);
char *pmc_utf_jsval_getstring(JSContext *cx, jsval val, size_t *u8len_r,
  const char *debugid); /* use pmc_free() to destroy */
JSBool pmc_utf_jsval_setstring(JSContext *cx, const char *str, size_t slen,
  jsval *rval);

#ifdef __cplusplus
}
#endif

#endif

