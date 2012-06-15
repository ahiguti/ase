
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef PMCJS_IMPL_HPP
#define PMCJS_IMPL_HPP

#include <stdlib.h>
#include <assert.h>
#define PERL_NO_GET_CONTEXT
#define MULTIPLICITY
#include <EXTERN.h>   /* Perl */
#include <perl.h>

#include "pmcjs.h"
#include "poutil_js.h"
#include "polist.h"
#include "pjsobject_js.h"
#include "pjsglobal.h"

/* internal data and functions */

#ifdef __cplusplus
extern "C" {
#endif

PerlInterpreter *pmc_get_interpreter(PMCPerl *ppl);
JSContext *pmc_get_current_jsctx(PMCPerl *ppl);
JSContext *pmc_enter_pmcperl(PMCPerl *ppl, JSContext *newjsctx);
  /* returns old jsctx */
void pmc_leave_pmcperl(PMCPerl *ppl, JSContext *oldjsctx);
pmc_listnode *pmc_register_perl_proxy(PMCPerl *ppl, JSContext *cx,
  JSObject *obj);
  /* returns NULL if pmc_malloc failed */
void pmc_unregister_perl_proxy(PMCPerl *ppl, pmc_listnode *nd);
void pmc_unregister_perl_proxy_nolock(PMCPerl *ppl, pmc_listnode *nd);
pmc_listnode *pmc_register_js_proxy(PMCPerl *ppl, JSContext *cx,
  pmc_jsobject *pjo);
  /* returns NULL if pmc_malloc failed */
void pmc_unregister_js_proxy(PMCPerl *ppl, pmc_listnode *nd);
void pmc_detach_perl_proxies(PMCPerl *ppl, JSContext *cx);
void pmc_detach_finalized_perl_proxies(PMCPerl *ppl, JSContext *cx);
void pmc_moveto_finalized_list_noperllock(PMCPerl *ppl, JSContext *cx,
  pmc_listnode *nd, void /* pmc_plpriv */ *pp);
void pmc_clean_finalized(PMCPerl *ppl, JSContext *cx);
void pmc_detach_js_proxies(PMCPerl *ppl, JSContext *cx);
pmc_jsglobal *pmc_get_pmc_jsglobal(PMCPerl *ppl);
void pmc_load_javascript_util(PMCPerl *ppl, JSContext *cx);
void pmc_load_javascript_objwrap(PMCPerl *ppl, JSContext *cx);
void pmc_load_javascript_core(PMCPerl *ppl, JSContext *cx);
JSBool pmc_eval_internal(pTHX_ PMCPerl *ppl, JSContext *cx, const char *str,
  const char *filename /* nullable */, jsval *rv /* nullable */);
void pmc_set_suppress_conv(PMCPerl *ppl, int suppress);
int pmc_get_suppress_conv(PMCPerl *ppl);
int pmc_remeber_loaded(PMCPerl *ppl, const char *name);

#ifdef __cplusplus
}
#endif

#endif

