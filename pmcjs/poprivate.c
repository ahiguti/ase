
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include "poprivate.h"
#include "pmconv.h"

#define DBG_CNT(x)
#define DBG_MEM(x)

struct pmc_plpriv_s {
  PMCPerl *pmcperl;
  SV *sv;
  pmc_listnode *node;
  int iterator_is_in_use : 1;
  pmc_plpriv_calltype calltype;
  char *pkgname;
  char *funcname;
};

pmc_plpriv *
pmc_plpriv_new(PMCPerl *ppl, JSContext *cx)
{
  pmc_plpriv *pp = (pmc_plpriv *)pmc_malloc(cx, sizeof(*pp), "pmc_plpriv_new");
  if (pp) {
    pp->pmcperl = ppl;
    pp->sv = NULL;
    pp->node = NULL;
    pp->iterator_is_in_use = 0;
    pp->calltype = pmc_plpriv_calltype_none;
    pp->pkgname = NULL;
    pp->funcname = NULL;
  }
  DBG_MEM(fprintf(stderr, "__spriv %p\n", pp));
  return pp;
}

static HV *
pmc_plpriv_get_hv(pmc_plpriv *pp)
{
  PerlInterpreter *my_perl;
  SV *sv, *svtarget;
  assert(pp);
  my_perl = pmc_get_interpreter(pp->pmcperl);
  sv = pp->sv;
  if (!sv) {
    return NULL;
  }
  if (!SvROK(sv)) {
    return NULL;
  }
  svtarget = SvRV(sv);
  if (!svtarget) {
    return NULL;
  }
  if (SvTYPE(svtarget) != SVt_PVHV) {
    return NULL;
  }
  return (HV *)svtarget;
}

int
pmc_plpriv_iterator_first(pmc_plpriv *pp)
{
  HV *hv;
  PerlInterpreter *my_perl;
  if (!pp) {
    return 0;
  }
  if (pp->iterator_is_in_use) {
    return 1;
  }
  hv = pmc_plpriv_get_hv(pp);
  if (!hv) {
    return -1;
  }
  my_perl = pmc_get_interpreter(pp->pmcperl);
  hv_iterinit(hv);
  pp->iterator_is_in_use = 1;
  return 0;
}

int
pmc_plpriv_iterator_next(pmc_plpriv *pp, HV **rhv, HE **rhe)
{
  PerlInterpreter *my_perl;
  HV *hv;
  HE *he;
  (*rhv) = NULL;
  (*rhe) = NULL;
  if (!pp) {
    return 0;
  }
  if (!pp->iterator_is_in_use) {
    return 0;
  }
  hv = pmc_plpriv_get_hv(pp);
  if (!hv) {
    return -1;
  }
  my_perl = pmc_get_interpreter(pp->pmcperl);
  he = hv_iternext(hv);
  if (!he) {
    pp->iterator_is_in_use = 0;
    return 0;
  }
  (*rhv) = hv;
  (*rhe) = he;
  return 1;
}

static void
pmc_plpriv_clear_iterator_state(pmc_plpriv *pp)
{
  if (!pp) {
    return;
  }
  if (pp->iterator_is_in_use) {
    /* we need call hv_iternext in order to deallocate the current entry.
      see 'man perlapi'. */
    HV *hv;
    HE *he;
    pmc_plpriv_iterator_next(pp, &hv, &he);
    pp->iterator_is_in_use = 0;
  }
}

void
pmc_plpriv_destroy(pmc_plpriv *pp)
{
  pmc_plpriv_clear_iterator_state(pp);
  /* 'perl' is null if it is closed already. if so, don't touch pp->sv. */
  if (pp && pp->sv) {
    PerlInterpreter *my_perl;
    my_perl = pmc_get_interpreter(pp->pmcperl);
    DBG_CNT(fprintf(stderr, "REFCNT dec: %p %d\n", pp->sv,
      (int)(SvREFCNT(pp->sv))));
    SvREFCNT_dec(pp->sv);
  }
  if (pp && pp->pkgname) {
    pmc_free(pp->pkgname, "pmc_plpriv_destroy");
  }
  if (pp && pp->funcname) {
    pmc_free(pp->funcname, "pmc_plpriv_destroy");
  }
  if (pp) {
    DBG_MEM(fprintf(stderr, "__fpriv %p\n", pp));
  }
  pmc_free(pp, "pmc_plpriv_destroy");
}

PMCPerl *
pmc_plpriv_get_pmcperl(pmc_plpriv *pp)
{
  return pp ? pp->pmcperl : NULL;
}

void
pmc_plpriv_set_sv(pmc_plpriv *pp, SV *sv)
{
  PerlInterpreter *my_perl;
  if (!pp) {
    return;
  }
  pmc_plpriv_clear_iterator_state(pp);
  my_perl = pmc_get_interpreter(pp->pmcperl);
  if (sv) {
    SvREFCNT_inc(sv);
    DBG_CNT(fprintf(stderr, "REFCNT inc: %p %d\n", sv, (int)(SvREFCNT(sv))));
  }
  if (pp->sv) {
    DBG_CNT(fprintf(stderr, "REFCNT dec: %p %d\n", pp->sv,
      (int)(SvREFCNT(pp->sv))));
    SvREFCNT_dec(pp->sv);
  }
  pp->sv = sv;
}

SV *
pmc_plpriv_get_sv(pmc_plpriv *pp)
{
  if (!pp) {
    return NULL;
  }
  return pp->sv;
}

void
pmc_plpriv_set_listnode(pmc_plpriv *pp, pmc_listnode *nd)
{
  if (!pp) {
    return;
  }
  pp->node = nd;
}

pmc_listnode *
pmc_plpriv_get_listnode(pmc_plpriv *pp)
{
  if (!pp) {
    return NULL;
  }
  return pp->node;
}

void
pmc_plpriv_set_calltype(pmc_plpriv *pp, pmc_plpriv_calltype ct)
{
  if (!pp) {
    return;
  }
  pp->calltype = ct;
}

pmc_plpriv_calltype
pmc_plpriv_get_calltype(pmc_plpriv *pp)
{
  return pp ? pp->calltype : pmc_plpriv_calltype_none;
}

const char *
pmc_plpriv_set_package(pmc_plpriv *pp, JSContext *cx, const char *pkgname)
{
  if (!pp) {
    return NULL;
  }
  if (pp->pkgname) {
    pmc_free(pp->pkgname, "pmc_plpriv_set_package");
    pp->pkgname = NULL;
  }
  pp->pkgname = pmc_strdup(cx, pkgname, "pmc_plpriv_set_package");
  return pp->pkgname;
}

const char *
pmc_plpriv_get_package(pmc_plpriv *pp)
{
  if (!pp) {
    return NULL;
  }
  return pp->pkgname;
}

const char *
pmc_plpriv_set_function(pmc_plpriv *pp, JSContext *cx, const char *funcname)
{
  if (!pp) {
    return NULL;
  }
  if (pp->funcname) {
    pmc_free(pp->funcname, "pmc_plpriv_set_function");
    pp->funcname = NULL;
  }
  pp->funcname = pmc_strdup(cx, funcname, "pmc_plpriv_set_function");
  return pp->funcname;
}

const char *
pmc_plpriv_get_function(pmc_plpriv *pp)
{
  if (!pp) {
    return NULL;
  }
  return pp->funcname;
}

