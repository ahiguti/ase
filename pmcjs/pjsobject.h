
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef PMCJS_PJSOBJECT_HPP
#define PMCJS_PJSOBJECT_HPP

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pmc_jsobject_s pmc_jsobject;
pmc_jsobject *pmc_jsobject_new();
void pmc_jsobject_destroy(pTHX_ pmc_jsobject *pjo);
SV *pmc_jsobject_call(pTHX_ SV *jsctx, int new_flag, int argc, SV **argv,
  const char **errstr_r, char **exception_allocated_r
  /* caller must free this string */);
SV *pmc_jsobject_eval(pTHX_ SV *jsctx, const char *str, const char **errstr_r,
  char **exception_allocated_r /* caller must free this string */);
SV *pmc_jsobject_invoke(pTHX_ pmc_jsobject *pjo, const char *methodname,
  int is_utf8, int argc, SV **argv, const char **errstr_r,
  char **exception_allocated_r /* caller must free this string */);
SV *pmc_jsobject_getprop(pTHX_ pmc_jsobject *pjo, const char *propname,
  int is_utf8, const char **errstr_r);
SV *pmc_jsobject_getelem(pTHX_ pmc_jsobject *pjo, int idx,
  const char **errstr_r);
const char *pmc_jsobject_setprop(pTHX_ pmc_jsobject *pjo,
  const char *propname, int is_utf8, SV *val);
const char *pmc_jsobject_setelem(pTHX_ pmc_jsobject *pjo, int idx, SV *val);
const char *pmc_jsobject_delprop(pTHX_ pmc_jsobject *pjo,
  const char *propname, int is_utf8);
const char *pmc_jsobject_delelem(pTHX_ pmc_jsobject *pjo, int idx);
SV *pmc_jsobject_getlength(pTHX_ pmc_jsobject *pjo);
const char *pmc_jsobject_setlength(pTHX_ pmc_jsobject *pjo, int len);
const char *pmc_jsobject_resetiterator(pTHX_ pmc_jsobject *pjo);
SV *pmc_jsobject_iterate(pTHX_ pmc_jsobject *pjo);

#ifdef __cplusplus
}
#endif

#endif

