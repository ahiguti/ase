
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef PMCJS_PO_PRIVATE_HPP
#define PMCJS_PO_PRIVATE_HPP

#include "pmcjs_impl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  pmc_plpriv_calltype_none = 0,
  pmc_plpriv_calltype_methodsv,
  pmc_plpriv_calltype_pkgfunc,
  pmc_plpriv_calltype_pkgconst,
  pmc_plpriv_calltype_pkgmethod,
  pmc_plpriv_calltype_pkgfunc_listcontext,
  pmc_plpriv_calltype_pkgconst_listcontext,
  pmc_plpriv_calltype_pkgmethod_listcontext,
  pmc_plpriv_calltype_pkgvariable,
  pmc_plpriv_calltype_pkgarray,
  pmc_plpriv_calltype_pkghash,
  pmc_plpriv_calltype_pkghandle,
  pmc_plpriv_calltype_pkgrawfunc,
} pmc_plpriv_calltype;

typedef struct pmc_plpriv_s pmc_plpriv;
pmc_plpriv *pmc_plpriv_new(PMCPerl *ppl, JSContext *cx);
void pmc_plpriv_destroy(pmc_plpriv *pp);
PMCPerl *pmc_plpriv_get_pmcperl(pmc_plpriv *pp);
void pmc_plpriv_set_sv(pmc_plpriv *pp, SV *sv);
SV *pmc_plpriv_get_sv(pmc_plpriv *pp);
void pmc_plpriv_set_listnode(pmc_plpriv *pp, pmc_listnode *nd);
pmc_listnode *pmc_plpriv_get_listnode(pmc_plpriv *pp);
int pmc_plpriv_iterator_first(pmc_plpriv *pp);
  /* -1 if it's not a container, 1 if iter is already in use, 0 on success. */
int pmc_plpriv_iterator_next(pmc_plpriv *pp, HV **rhv, HE **rhe);
  /* -1 if it's not a container, 1 if we get an element. 0 if no more
    element is available or iteration is not in progress. */
/* calltype, package, and function are used for Perl.Functions["..."]. */
void pmc_plpriv_set_calltype(pmc_plpriv *pp, pmc_plpriv_calltype ct);
pmc_plpriv_calltype pmc_plpriv_get_calltype(pmc_plpriv *pp);
const char *pmc_plpriv_set_package(pmc_plpriv *pp, JSContext *cx,
  const char *pkgname);
const char *pmc_plpriv_get_package(pmc_plpriv *pp);
const char *pmc_plpriv_set_function(pmc_plpriv *pp, JSContext *cx,
  const char *funcname);
const char *pmc_plpriv_get_function(pmc_plpriv *pp);

#ifdef __cplusplus
}
#endif

#endif

