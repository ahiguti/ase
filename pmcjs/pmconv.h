
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef PMCJS_PMCONV_HPP
#define PMCJS_PMCONV_HPP

#include "pmcjs_impl.h"
#include "poprivate.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  pmc_tojs_mode_auto,
  pmc_tojs_mode_binary,
  pmc_tojs_mode_text,
  pmc_tojs_mode_keep
} pmc_tojs_mode;

JSBool pmc_funccall_internal(pTHX_ JSContext *cx, int argc, SV **argv,
  int *rvalcp, SV ***rvalvp, const char *funcname, int is_method,
  I32 evalctx);
JSBool pmc_funccall_sv_internal(pTHX_ JSContext *cx, int argc, SV **argv,
  int *rvalcp, SV ***rvalvp, SV *func, I32 evalctx);
JSBool pmc_funccall_svorpv_internal(pTHX_ JSContext *cx, int argc, SV **argv,
  int *rvalcp, SV ***rvalvp, SV *funcsv, const char *funcname,
  int is_method, I32 evalctx);
void pmc_free_svarr(pTHX_ int num, SV **arr, const char *debugid);
void pmc_free_svarr_contentsonly(pTHX_ int num, SV **arr);
pmc_plpriv *pmc_plproxy_get_plpriv(JSContext *cx, JSObject *obj);
JSBool pmc_plproxy_attach_plpriv(pTHX_ PMCPerl *ppl, JSContext *cx,
  JSObject *obj, SV *sv);
void pmc_plproxy_detach_plpriv(pTHX_ PMCPerl *ppl, JSContext *cx,
  JSObject *obj);
void pmc_plproxy_detach_plpriv_nolock(pTHX_ PMCPerl *ppl, JSContext *cx,
  JSObject *obj);
void pmc_plproxy_detach_plpriv_moveto_finalized_list_noperllock(pTHX_
  PMCPerl *ppl, JSContext *cx, JSObject *obj);
SV *pmc_jsval_to_sv(pTHX_ PMCPerl *ppl, JSContext *cx, jsval jv,
  int binary_flag);
JSBool pmc_sv_to_double(JSContext *cx, double dv, jsval *rval);
JSBool pmc_sv_to_int(JSContext *cx, IV iv, jsval *rval);
JSBool pmc_sv_to_wrappedsv(pTHX_ PMCPerl *ppl, JSContext *cx, SV *sv,
  jsval *rval);
JSBool pmc_sv_to_string(pTHX_ JSContext *cx, pmc_tojs_mode mode,
  SV *sv /* must SvPOK */, jsval *rval);
JSBool pmc_sv_to_jsval(pTHX_ PMCPerl *ppl, JSContext *cx, pmc_tojs_mode mode,
  SV *sv, jsval *rval);
JSBool pmc_sv_to_jsval_array(pTHX_ PMCPerl *ppl, JSContext *cx,
  pmc_tojs_mode mode, int rvalc, SV **rvalv, jsval *rval);
JSBool pmc_plproxy_new_pkgobject(pTHX_ PMCPerl *ppl, JSContext *cx,
  pmc_plpriv_calltype ct, const char *pkgname, const char *funcname,
  jsval *rval);
JSBool pmc_plproxy_new_pkgobject_stash(pTHX_ PMCPerl *ppl, JSContext *cx,
  pmc_plpriv_calltype ct, const char *pkgname, jsval *rval);
JSBool pmc_plproxy_new_methodsv(pTHX_ PMCPerl *ppl, JSContext *cx,
  SV *sv, JSObject *parent, jsval *rval);
JSBool pmc_plproxy_getvar_internal(pTHX_ PMCPerl *ppl, JSContext *cx,
  const char *name, jsval *vp);
JSBool pmc_plproxy_setvar_internal(pTHX_ PMCPerl *ppl, JSContext *cx,
  const char *name, jsval *vp);
pmc_tojs_mode pmc_plproxy_get_tojs_current_mode(PMCPerl *ppl);


#ifdef __cplusplus
}
#endif

#endif

