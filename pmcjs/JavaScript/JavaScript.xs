
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A. Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifdef __GNUC__
#include <alloca.h>
#define MACRO_ALLOCA(x) alloca(x)
#elif defined(_MSC_VER)
#include <malloc.h>
#define MACRO_ALLOCA(x) _alloca(x)
#endif

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "ppport.h"
#include "pjsobject.h"
#include "poutil.h"

#define DBG_JSPM(x)
#define DBG_ROOT(x)
#define DBG_MEM(x)


#define PMCJS_JS_ALLOCATE_MAX 1024

/* copies exception_allocated from heap to stack. because croak() longjumps,
  we need to copy it to stack in order not to leak it. */
#define COPY_AND_DESTROY_EXCEPTION_STR \
  if (exception_allocated) { \
    size_t len; \
    char *p; \
    len = strlen(exception_allocated); \
    len = len < PMCJS_JS_ALLOCATE_MAX ? len : PMCJS_JS_ALLOCATE_MAX; \
    p = MACRO_ALLOCA(len + 1); \
    memcpy(p, exception_allocated, len); \
    p[len] = '\0'; \
    pmc_free(exception_allocated, "JS::copyexceptionstr"); \
    exception_allocated = p; \
  }


static pmc_jsobject *
pmc_get_rep(SV *obj)
{
  SV *svtarget;
  pmc_jsobject *r;
  if (!SvROK(obj)) {
    croak("JavaScript: wrong object");
    return NULL;
  }
  svtarget = SvRV(obj);
  if (!svtarget || !SvIOKp(svtarget)) {
    croak("JavaScript: wrong object");
    return NULL;
  }
  r = (pmc_jsobject *)SvIV(svtarget);
  if (!r) {
    croak("JavaScript: wrong object");
  }
  return r;
}

MODULE = JavaScript  PACKAGE = JavaScript

void
LoadModule(name)
  SV *name
CODE:
  SvREFCNT_inc(name);
  load_module(PERL_LOADMOD_NOIMPORT, name, Nullsv, Nullsv);

void
DESTROY(obj)
  SV *obj
CODE:
  pmc_jsobject *rep;
  DBG_JSPM(fprintf(stderr, "JavaScript::DESTROY\n"));
  rep = pmc_get_rep(obj);
  pmc_jsobject_destroy(aTHX_ rep);

SV *
NewPassive(klass)
  const char *klass
CODE:
  pmc_jsobject *rep;
  SV *svret = NULL;
  DBG_JSPM(fprintf(stderr, "JavaScript::NewPassive\n"));
  rep = pmc_jsobject_new();
  if (!rep) {
    svret = newSV(0);
  } else {
    SV *obj, *objref;
    objref = newSViv(0);
    obj = newSVrv(objref, klass);
    sv_setiv(obj, (IV)rep);
    SvREADONLY_on(obj);
    svret = objref;
  }
  RETVAL = svret ? svret : &PL_sv_undef;
OUTPUT:
  RETVAL

SV *
new(klass, ...)
  char *klass
CODE:
  SV *jsctx;
  SV *svret = NULL;
  const char *errstr = NULL;
  char *exception_allocated = NULL;
  DBG_JSPM(fprintf(stderr, "JavaScript::new\n"));
  (void)klass;
  jsctx = get_sv("JavaScript::PMCGlobal", FALSE);
  if (jsctx) {
    int argc;
    SV **args;
    argc = items - 1;
    args = (SV **)malloc(argc ? (argc * sizeof(SV *)) : 1);
    if (args) {
      int i;
      for (i = 0; i < argc; ++i) {
	DBG_JSPM(fprintf(stderr, "JavaScript::new arg(%d)\n", i));
	args[i] = ST(i + 1);
      }
      svret = pmc_jsobject_call(aTHX_ jsctx, 1, argc, args, &errstr,
	&exception_allocated);
      COPY_AND_DESTROY_EXCEPTION_STR;
    }
    free(args);
  }
  if (!svret && (errstr || exception_allocated)) {
    croak(exception_allocated ? exception_allocated : errstr);
  }
  RETVAL = svret ? svret : &PL_sv_undef;
OUTPUT:
  RETVAL

SV *
Call(klass, ...)
  const char *klass
CODE:
  SV *jsctx;
  SV *svret = NULL;
  const char *errstr = NULL;
  char *exception_allocated = NULL;
  DBG_JSPM(fprintf(stderr, "JavaScript::Call\n"));
  (void)klass;
  jsctx = get_sv("JavaScript::PMCGlobal", FALSE);
  if (jsctx) {
    int argc;
    SV **args;
    argc = items - 1;
    args = (SV **)malloc(argc ? (argc * sizeof(SV *)) : 1);
    if (args) {
      int i;
      for (i = 0; i < argc; ++i) {
	DBG_JSPM(fprintf(stderr, "JavaScript::Call arg(%d)\n", i));
	args[i] = ST(i + 1);
      }
      svret = pmc_jsobject_call(aTHX_ jsctx, 0, argc, args, &errstr,
	&exception_allocated);
      COPY_AND_DESTROY_EXCEPTION_STR;
    }
    free(args);
  }
  if (!svret && (errstr || exception_allocated)) {
    croak(exception_allocated ? exception_allocated : errstr);
  }
  RETVAL = svret ? svret : &PL_sv_undef;
OUTPUT:
  RETVAL

SV *
Eval(klass, str)
  const char *klass
  const char *str
CODE:
  SV *jsctx;
  SV *svret = NULL;
  const char *errstr = NULL;
  char *exception_allocated = NULL;
  DBG_JSPM(fprintf(stderr, "JavaScript::Call\n"));
  (void)klass;
  jsctx = get_sv("JavaScript::PMCGlobal", FALSE);
  if (jsctx) {
    svret = pmc_jsobject_eval(aTHX_ jsctx, str, &errstr, &exception_allocated);
    COPY_AND_DESTROY_EXCEPTION_STR;
  }
  if (!svret && (errstr || exception_allocated)) {
    croak(exception_allocated ? exception_allocated : errstr);
  }
  RETVAL = svret ? svret : &PL_sv_undef;
OUTPUT:
  RETVAL

SV *
GetProperty(obj, propname)
  SV *obj
  SV *propname
CODE:
  pmc_jsobject *rep;
  const char *pnstr;
  STRLEN pnlen;
  const char *errstr = NULL;
  SV *svret;
  rep = pmc_get_rep(obj);
  pnstr = SvPV(propname, pnlen);
  DBG_JSPM(fprintf(stderr, "JavaScript::GetProperty [%s]\n", pnstr));
  svret = pmc_jsobject_getprop(aTHX_ rep, pnstr, SvUTF8(propname), &errstr);
  if (!svret && errstr) {
    croak(errstr);
  }
  RETVAL = svret ? svret : &PL_sv_undef;
OUTPUT:
  RETVAL

SV *
GetElement(obj, idx)
  SV *obj
  int idx
CODE:
  pmc_jsobject *rep;
  const char *errstr = NULL;
  SV *svret;
  rep = pmc_get_rep(obj);
  DBG_JSPM(fprintf(stderr, "JavaScript::GetElement %d\n", idx));
  svret = pmc_jsobject_getelem(aTHX_ rep, idx, &errstr);
  if (!svret && errstr) {
    croak(errstr);
  }
  RETVAL = svret ? svret : &PL_sv_undef;
OUTPUT:
  RETVAL

void
SetProperty(obj, propname, val)
  SV *obj
  SV *propname
  SV *val
CODE:
  pmc_jsobject *rep;
  const char *pnstr;
  STRLEN pnlen;
  const char *errstr = NULL;
  rep = pmc_get_rep(obj);
  pnstr = SvPV(propname, pnlen);
  DBG_JSPM(fprintf(stderr, "JavaScript::SetProperty [%s]\n", pnstr));
  errstr = pmc_jsobject_setprop(aTHX_ rep, pnstr, SvUTF8(propname), val);
  if (errstr) {
    croak(errstr);
  }

void
SetElement(obj, idx, val)
  SV *obj
  int idx
  SV *val
CODE:
  pmc_jsobject *rep;
  const char *errstr = NULL;
  rep = pmc_get_rep(obj);
  DBG_JSPM(fprintf(stderr, "JavaScript::SetElement %d\n", idx));
  errstr = pmc_jsobject_setelem(aTHX_ rep, idx, val);
  if (errstr) {
    croak(errstr);
  }

void
DelProperty(obj, propname)
  SV *obj
  SV *propname
CODE:
  pmc_jsobject *rep;
  const char *pnstr;
  STRLEN pnlen;
  const char *errstr = NULL;
  rep = pmc_get_rep(obj);
  pnstr = SvPV(propname, pnlen);
  DBG_JSPM(fprintf(stderr, "JavaScript::DelProperty [%s]\n", pnstr));
  errstr = pmc_jsobject_delprop(aTHX_ rep, pnstr, SvUTF8(propname));
  if (errstr) {
    croak(errstr);
  }

void
DelElement(obj, idx)
  SV *obj
  int idx
CODE:
  pmc_jsobject *rep;
  const char *errstr = NULL;
  rep = pmc_get_rep(obj);
  DBG_JSPM(fprintf(stderr, "JavaScript::DelElement %d\n", idx));
  errstr = pmc_jsobject_delelem(aTHX_ rep, idx);
  if (errstr) {
    croak(errstr);
  }

SV *
GetLength(obj)
  SV *obj
CODE:
  pmc_jsobject *rep;
  SV *svret;
  rep = pmc_get_rep(obj);
  DBG_JSPM(fprintf(stderr, "JavaScript::GetLength\n"));
  svret = pmc_jsobject_getlength(aTHX_ rep);
  if (!svret) {
    croak("JavaScript::GetLength failed");
  }
  RETVAL = svret ? svret : &PL_sv_undef;
OUTPUT:
  RETVAL

void
SetLength(obj, len)
  SV *obj
  int len
CODE:
  pmc_jsobject *rep;
  const char *errstr = NULL;
  rep = pmc_get_rep(obj);
  DBG_JSPM(fprintf(stderr, "JavaScript::SetLength %d\n", len));
  errstr = pmc_jsobject_setlength(aTHX_ rep, len);
  if (errstr) {
    croak(errstr);
  }

void
ResetIterator(obj)
  SV *obj
CODE:
  pmc_jsobject *rep;
  const char *errstr;
  rep = pmc_get_rep(obj);
  errstr = pmc_jsobject_resetiterator(aTHX_ rep);
  if (errstr) {
    croak(errstr);
  }

SV *
Iterate(obj, ...)
  SV *obj
CODE:
  pmc_jsobject *rep;
  SV *svret;
  rep = pmc_get_rep(obj);
  svret = pmc_jsobject_iterate(aTHX_ rep);
  RETVAL = svret ? svret : &PL_sv_undef;
OUTPUT:
  RETVAL

SV *
Invoke(obj, methodname, ...)
  SV *obj
  SV *methodname
CODE:
  pmc_jsobject *rep;
  const char *mnstr;
  STRLEN mnlen;
  int argc;
  SV **args;
  const char *errstr = NULL;
  char *exception_allocated = NULL;
  SV *svret = NULL;
  rep = pmc_get_rep(obj);
  mnstr = SvPV(methodname, mnlen);
  DBG_JSPM(fprintf(stderr, "JavaScript::Invoke [%s]\n", mnstr));
  argc = items - 2;
  args = (SV **)malloc(argc ? (argc * sizeof(SV *)) : 1);
  if (args) {
    int i = 0;
    const char *name = NULL;
    const char *colonpos = NULL;
    for (i = 0; i < argc; ++i) {
      DBG_JSPM(fprintf(stderr, "JavaScript::Invoke arg(%d)\n", i));
      args[i] = ST(i + 2);
    }
    name = mnstr;
    colonpos = strrchr(name, ':');
    if (colonpos) {
      name = colonpos + 1;
    }
    svret = pmc_jsobject_invoke(aTHX_ rep, name, SvUTF8(methodname),
      argc, args, &errstr, &exception_allocated);
    free(args);
    COPY_AND_DESTROY_EXCEPTION_STR;
    if (!svret && (errstr || exception_allocated)) {
      croak(exception_allocated ? exception_allocated : errstr);
    }
  } else {
    croak("JavaScript::Invoke: malloc failed");
  }
  RETVAL = svret ? svret : &PL_sv_undef;
OUTPUT:
  RETVAL

SV *
InvokeSelf(obj, ...)
  SV *obj
CODE:
  pmc_jsobject *rep;
  int argc;
  SV **args;
  const char *errstr = NULL;
  char *exception_allocated = NULL;
  SV *svret = NULL;
  rep = pmc_get_rep(obj);
  DBG_JSPM(fprintf(stderr, "JavaScript::InvokeSelf\n"));
  argc = items - 1;
  args = (SV **)malloc(argc ? (argc * sizeof(SV *)) : 1);
  if (args) {
    int i = 0;
    for (i = 0; i < argc; ++i) {
      DBG_JSPM(fprintf(stderr, "JavaScript::InvokeSelf arg(%d)\n", i));
      args[i] = ST(i + 1);
    }
    svret = pmc_jsobject_invoke(aTHX_ rep, NULL, 0,
      argc, args, &errstr, &exception_allocated);
    free(args);
    COPY_AND_DESTROY_EXCEPTION_STR;
    if (!svret && (errstr || exception_allocated)) {
      croak(exception_allocated ? exception_allocated : errstr);
    }
  } else {
    croak("JavaScript::InvokeSelf: malloc failed");
  }
  RETVAL = svret ? svret : &PL_sv_undef;
OUTPUT:
  RETVAL

