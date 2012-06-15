
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef PMCJS_PJSOBJECT_JS_HPP
#define PMCJS_PJSOBJECT_JS_HPP

#include "pjsobject.h"

#ifdef __cplusplus
extern "C" {
#endif

JSBool pmc_jsobject_attach_js(pTHX_ PMCPerl *ppl, pmc_jsobject *pjo,
  JSContext *cx, JSObject *obj);
  /* returns JS_FALSE if addroot failed (out of memory) */
void pmc_jsobject_detach_js(pTHX_ pmc_jsobject *pjo, JSContext *cx);
JSObject *pmc_jsobject_get_js(pmc_jsobject *pjo);

#ifdef __cplusplus
}
#endif

#endif

