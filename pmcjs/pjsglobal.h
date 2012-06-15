
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef PMCJS_PJSGLOBAL_HPP
#define PMCJS_PJSGLOBAL_HPP

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pmc_jsglobal_s pmc_jsglobal;
pmc_jsglobal *pmc_jsglobal_new(pTHX_ PMCPerl *ppl, JSContext *cx,
  JSObject *globj);
void pmc_jsglobal_destroy(pTHX_ pmc_jsglobal *pjg);
SV *pmc_jsglobal_getsv(pmc_jsglobal *pjg);
pmc_jsglobal *pmc_jsglobal_fromsv(pTHX_ SV *sv);
PMCPerl *pmc_jsglobal_getpmcperl(pmc_jsglobal *pjg);
JSContext *pmc_jsglobal_getcontext(pmc_jsglobal *pjg);
JSObject *pmc_jsglobal_getobject(pmc_jsglobal *pjg);

#ifdef __cplusplus
}
#endif

#endif

