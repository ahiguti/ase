
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef PMCJS_POUTIL_JS_HPP
#define PMCJS_POUTIL_JS_HPP

#include "poutil.h"
#include <jsapi.h>

#ifdef __cplusplus
extern "C" {
#endif

void *pmc_malloc(JSContext *cx, size_t len, const char *debugid);
char *pmc_strdup(JSContext *cx, const char *str, const char *debugid);
char *pmc_strcat(JSContext *cx, const char *str1, const char *str2,
  const char *debugid);

JSBool pmc_throw_js_logic_error(JSContext *cx, const char *err,
  const char *func, const char *description);
JSBool pmc_throw_js_runtime_exception(JSContext *cx, const char *err,
  const char *func, const char *description);
JSBool pmc_report_js_outofmemory(JSContext *cx);
JSBool pmc_throw_js_nopmc(JSContext *cx, const char *func);

void pmc_dump_jsval(JSContext *cx, const char *mess, jsval jv);

#ifdef __cplusplus
}
#endif

#endif

