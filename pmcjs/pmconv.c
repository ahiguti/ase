
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include "poprivate.h"
#include "utfutil.h"
#include "poutil_js.h"
#include "pjsobject_js.h"
#include "pmobject.h"
#include "pmconv.h"
#include "pmutf.h"
#include <jsfun.h>

#define DBG(x)
#define DBG_PRIV(x)
#define DBG_INT(x)
#define DBG_ARR(x)
#define DBG_LEN(x)
#define DBG_READONLY(x)
#define DBG_FETCH(x)
#define DBG_ARRCTX(x)
#define DBG_UTFERR(x)
#define DBG_UTF(x)
#define DBG_CONV(x)
#define DBG_CREATE(x)
#define DBG_WRAP(x)
#define DBG_AUTO(x)
#define DBG_PKG(x)
#define DBG_EXTRACT(x)
#define DBG_SETSV(x)
#define DBG_UL(x)

JSBool
pmc_funccall_internal(pTHX_ JSContext *cx, int argc, SV **argv,
  int *rvalcp, SV ***rvalvp, const char *funcname, int is_method, I32 evalctx)
{
  return pmc_funccall_svorpv_internal(aTHX_ cx, argc, argv, rvalcp, rvalvp,
    NULL, funcname, is_method, evalctx);
}

JSBool
pmc_funccall_sv_internal(pTHX_ JSContext *cx, int argc, SV **argv,
  int *rvalcp, SV ***rvalvp, SV *funcsv, I32 evalctx)
{
  return pmc_funccall_svorpv_internal(aTHX_ cx, argc, argv, rvalcp, rvalvp,
    funcsv, "", 0, evalctx);
}

JSBool
pmc_funccall_svorpv_internal(pTHX_ JSContext *cx, int argc, SV **argv,
  int *rvalcp, SV ***rvalvp, SV *funcsv, const char *funcname,
  int is_method, I32 evalctx)
{
  dSP;
  int i, rvalc;
  SV **rvalv;
  JSBool suc = JS_TRUE;
  (*rvalcp) = 0;
  (*rvalvp) = NULL;
  DBG(fprintf(stderr, "funccal internal\n"));
  ENTER;
  SAVETMPS;
  PUSHMARK(SP);
  for (i = 0; i < argc; ++i) {
    XPUSHs(argv[i]);
  }
  PUTBACK;
  evalctx |= G_EVAL;
  if (funcsv) {
    rvalc = call_sv(funcsv, evalctx);
  } else if (is_method) {
    rvalc = call_method(funcname, evalctx);
  } else {
    rvalc = call_pv(funcname, evalctx);
  }
  SPAGAIN;
  if (SvTRUE(ERRSV)) {
    char *estr;
    STRLEN elen;
    estr = SvPV(ERRSV, elen);
    pmc_throw_js_runtime_exception(cx, "PerlException",
      "pmc_funccall_internal", estr);
    DBG(fprintf(stderr, "perl call failed\n"));
    suc = JS_FALSE;
  }
  if (rvalc > 0) {
    rvalv = (SV **)pmc_malloc(cx, sizeof(*rvalv) * rvalc,
      "pmc_funccall_internal");
    if (rvalv) {
      for (i = 0; i < rvalc; ++i) {
	rvalv[i] = POPs;
	SvREFCNT_inc(rvalv[i]);
      }
      (*rvalcp) = rvalc;
      (*rvalvp) = rvalv;
    } else {
      for (i = 0; i < rvalc; ++i) {
	(void)POPs;
      }
      suc = JS_FALSE;
    }
  }
  PUTBACK;
  FREETMPS;
  LEAVE;
  return suc;
}

void
pmc_free_svarr_contentsonly(pTHX_ int num, SV **arr)
{
  int i;
  for (i = 0; i < num; ++i) {
    if (arr && arr[i]) {
      SvREFCNT_dec(arr[i]);
    }
  }
}

void
pmc_free_svarr(pTHX_ int num, SV **arr, const char *debugid)
{
  pmc_free_svarr_contentsonly(aTHX_ num, arr);
  pmc_free(arr, debugid);
}

pmc_plpriv *
pmc_plproxy_get_plpriv(JSContext *cx, JSObject *obj)
{
  /* we must check class before calling GetPrivate, or js aborts. */
  JSClass *cl;
  cl = JS_GET_CLASS(cx, obj);
  if (cl != &pmc_perl_class) {
    return NULL;
  }
  return (pmc_plpriv *)JS_GetPrivate(cx, obj);
}

static SV *
pmc_create_jsproxy(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj)
{
  JSBool suc = JS_TRUE;
  SV *cnamesv = NULL;
  int rvalc = 0;
  SV **rvalv = NULL;
  const int is_method = 1;
  SV *r = NULL;

  DBG_CREATE(fprintf(stderr, "pmc_create_jsproxy\n"));

  /* require JavaScript package */
  pmc_load_javascript_objwrap(ppl, cx);

  cnamesv = newSVpv("JavaScript", 0);
  suc = pmc_funccall_internal(aTHX_ cx, 1, &cnamesv, &rvalc, &rvalv,
    "NewPassive", is_method, G_SCALAR);
  if (rvalc > 0) {
    r = rvalv[0];
    if (r) {
      pmc_jsobject *rep;
      rep = (pmc_jsobject *)SvIV(SvRV(r));
      if (!pmc_jsobject_attach_js(aTHX_ ppl, rep, cx, obj)) {
	r = NULL;
      } else {
	SvREFCNT_inc(r);
      }
    }
  }
  SvREFCNT_dec(cnamesv);
  pmc_free_svarr(aTHX_ rvalc, rvalv, "pmc_create_jsproxy");
  return r;
}

static int
pmc_skip_paren_space(const char **bp, size_t *lp)
{
  const char *b;
  size_t l;
  int r = 0;
  b = (*bp);
  l = (*lp);
  while (l > 0) {
    char c;
    c = (*b);
    if (c == '(' || c == ' ' || c == '\t' || c == ')' || c == ',') {
      r = 1;
    } else {
      break;
    }
    ++b;
    --l;
  }
  (*bp) = b;
  (*lp) = l;
  return r;
}

static int
pmc_skip_noparen(const char **bp, size_t *lp)
{
  const char *b;
  size_t l;
  int r = 0;
  b = (*bp);
  l = (*lp);
  while (l > 0) {
    char c;
    c = (*b);
    if (c == '(' || c == ')') {
      break;
    } else {
      r = 1;
    }
    ++b;
    --l;
  }
  (*bp) = b;
  (*lp) = l;
  return r;
}

static int
pmc_read_token(const char **bp, size_t *lp, const char *tok)
{
  const char *b;
  size_t l, tl;
  b = (*bp);
  l = (*lp);
  tl = strlen(tok);
  if (l < tl) {
    return 0;
  }
  if (memcmp(b, tok, tl) == 0) {
    (*bp) = b + tl;
    (*lp) = l - tl;
    return 1;
  }
  return 0;
}

static SV *
pmc_create_jsproxy_fnc3rd(pTHX_ JSContext *cx, JSObject *obj)
{
  jsval jv = JSVAL_VOID;
  SV *sv = NULL;
  JSString *str = NULL;
  const char *bytes = NULL;
  size_t len = 0;

  if (!JS_AddNamedRoot(cx, &jv, "pmc_create_jsproxy_func3rd")) {
    return NULL;
  }
  if (!JS_CallFunctionName(cx, obj, "toSource", 0, NULL, &jv)) {
    sv = newSV(0);
    DBG_UL(fprintf(stderr, "ISUL: toSource failed\n"));
    goto out;
  }
  if (!JSVAL_IS_STRING(jv)) {
    sv = newSV(0);
    DBG_UL(fprintf(stderr, "ISUL: src is not string\n"));
    goto out;
  }
  str = JSVAL_TO_STRING(jv);
  bytes = JS_GetStringBytes(str);
  len = JS_GetStringLength(str);
  /* 'function(_)'  or 'function foo(_)' */
  pmc_skip_paren_space(&bytes, &len);
  if (!pmc_read_token(&bytes, &len, "function")) {
    sv = newSV(0);
    DBG_UL(fprintf(stderr, "ISUL: function not found\n"));
    goto out;
  }
  pmc_skip_noparen(&bytes, &len); /* skip function name */
  pmc_skip_paren_space(&bytes, &len);
  if (!pmc_read_token(&bytes, &len, "_")) {
    sv = newSV(0);
    DBG_UL(fprintf(stderr, "ISUL: not ul\n"));
    goto out;
  }
  if (!pmc_skip_paren_space(&bytes, &len)) {
    sv = newSV(0);
    DBG_UL(fprintf(stderr, "ISUL: long argname\n"));
    goto out;
  }
  DBG_UL(fprintf(stderr, "ISUL: ISUL\n"));
  sv = newSViv(1);

 out:
  JS_RemoveRoot(cx, &jv);
  return sv;
}

static SV *
pmc_create_jsproxy_wrap(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  const char *wrapname, int isfunc)
{
  JSBool suc = JS_TRUE;
  SV *inner = NULL;
  SV *cnamesv = NULL;
  int argc = 0, i = 0;
  SV **argv = NULL;
  int rvalc = 0;
  SV **rvalv = NULL;
  const int is_method = 1;
  SV *r = NULL;
  SV *fnc3rd = NULL;

  if (isfunc) {
    argc = 3;
  } else {
    argc = 2;
  }
  argv = pmc_malloc(cx, argc * sizeof(SV *), "pmc_create_jsproxy_wrap");
  if (!argv) {
    return NULL;
  }
  for (i = 0; i < argc; ++i) {
    argv[i] = NULL;
  }
  DBG_CREATE(fprintf(stderr, "pmc_create_jsproxy_wrap\n"));
  inner = pmc_create_jsproxy(aTHX_ ppl, cx, obj);
  if (!inner) {
    r = inner;
    goto end;
  }
  cnamesv = newSVpv(wrapname, 0);
  argv[0] = cnamesv;
  argv[1] = inner;
  if (isfunc) {
    fnc3rd = pmc_create_jsproxy_fnc3rd(aTHX_ cx, obj);
    if (!fnc3rd) {
      goto end;
    }
    argv[2] = fnc3rd;
  }
  suc = pmc_funccall_internal(aTHX_ cx, argc, argv, &rvalc, &rvalv,
    "new", is_method, G_SCALAR);
  if (rvalc > 0) {
    r = rvalv[0];
    if (r) {
      SvREFCNT_inc(r);
    }
  }

 end:

  pmc_free_svarr(aTHX_ argc, argv, "pmc_create_jsproxy_wrap");
  pmc_free_svarr(aTHX_ rvalc, rvalv, "pmc_create_jsproxy_wrap");
  return r;
}

SV *
pmc_jsval_to_sv(pTHX_ PMCPerl *ppl, JSContext *cx, jsval jv, int binary_flag)
{
  JSString *s;
  SV *sv = NULL;
  if (JSVAL_IS_INT(jv)) {
    DBG(fprintf(stderr, "tosv: int\n"));
    sv = newSViv(JSVAL_TO_INT(jv));
  } else if (JSVAL_IS_STRING(jv)) {
    s = JSVAL_TO_STRING(jv);
    DBG(fprintf(stderr, "tosv: string: %s\n", JS_GetStringBytes(s)));
    if (binary_flag) {
      sv = newSVpvn(JS_GetStringBytes(s), JS_GetStringLength(s));
      DBG_UTF(fprintf(stderr, "jstosv noutf\n"));
    } else {
      char *u8str = NULL;
      size_t u8len = 0;
      pmc_u16ch_t *u16str = NULL;
      size_t u16len = 0;
      u16str = JS_GetStringChars(s);
      u16len = JS_GetStringLength(s);
      {
	u8str = pmc_new_utf8str_from_utf16str(cx, u16str, u16len, &u8len,
	  "pmc_jsval_to_sv");
	if (!u8str) {
	  return NULL;
	}
	DBG_UTF(fprintf(stderr, "jstosv utf u16len=%d u8len=%d\n",
	  (int)u16len, (int)u8len));
	sv = newSVpvn(u8str, u8len);
	pmc_free(u8str, "pmc_jsval_to_sv");
      }
      if (sv) {
	SvUTF8_on(sv);
      }
    }
  } else if (JSVAL_IS_BOOLEAN(jv)) {
    if (jv == JSVAL_TRUE) {
      DBG(fprintf(stderr, "tosv: true\n"));
      sv = newSViv(1);
    } else {
      DBG(fprintf(stderr, "tosv: false\n"));
      sv = newSViv(0);
    }
  } else if (JSVAL_IS_NULL(jv) || JSVAL_IS_VOID(jv)) {
    DBG(fprintf(stderr, "tosv: void null\n"));
    sv = newSV(0);
  } else if (JSVAL_IS_DOUBLE(jv)) {
    DBG(fprintf(stderr, "tosv: double\n"));
    sv = newSVnv(*JSVAL_TO_DOUBLE(jv));
  } else if (JSVAL_IS_OBJECT(jv)) {
    JSObject *o;
    pmc_plpriv *pp;
    int is_asefunc = 0;
    int is_asearray = 0;
    o = JSVAL_TO_OBJECT(jv);
    pp = pmc_plproxy_get_plpriv(cx, o);
    if (!pp) {
      /* ASE proxy? */
      JSClass *cl;
      cl = JS_GET_CLASS(cx, o);
      if (strcmp(cl->name, "ASEVariant") == 0) {
	int aseattrib = 0;
	jsval jatr = JSVAL_VOID;
	if (!JS_GetProperty(cx, o, "__aseattr__", &jatr)) {
	  return NULL;
	}
	if (JSVAL_IS_INT(jatr)) {
	  aseattrib = JSVAL_TO_INT(jatr);
	}
	if ((aseattrib & 1) != 0) {
	  is_asefunc = 1;
	}
	if ((aseattrib & 2) != 0) {
	  is_asearray = 1;
	}
      }
    }
    if (pp) {
      pmc_plpriv_calltype ct = pmc_plpriv_calltype_none;
      ct = pmc_plpriv_get_calltype(pp);
      if (ct == pmc_plpriv_calltype_none) {
	/* js-wrapped perl object */
	sv = pmc_plpriv_get_sv(pp);
	DBG(fprintf(stderr, "tosv: scalar\n"));
	if (sv) {
	  SvREFCNT_inc(sv);
	}
      } else {
	const char *pkgname = NULL;
	const char *funcname = NULL;
	int callable = 0;
	pkgname = pmc_plpriv_get_package(pp);
	funcname = pmc_plpriv_get_function(pp);
	switch (ct) {
	case pmc_plpriv_calltype_pkgfunc:
	case pmc_plpriv_calltype_pkgmethod:
	case pmc_plpriv_calltype_pkgfunc_listcontext:
	case pmc_plpriv_calltype_pkgmethod_listcontext:
	  callable = 1;
	  break;
	default:
	  break;
	}
	if (pkgname && funcname && callable) {
	  sv = pmc_create_jsproxy_wrap(aTHX_ ppl, cx, o,
	    "JavaScript::CWrap", 1);
	} else {
	  sv = pmc_create_jsproxy_wrap(aTHX_ ppl, cx, o,
	    "JavaScript::HTWrap", 0);
	}
	if (!sv) {
	  /* already thrown */
	  return NULL;
	}
      }
    } else if (is_asefunc || JS_ObjectIsFunction(cx, o)) {
      DBG(fprintf(stderr, "tosv: function object\n"));
      sv = pmc_create_jsproxy_wrap(aTHX_ ppl, cx, o, "JavaScript::CWrap", 1);
      if (!sv) {
	/* already thrown */
	return NULL;
      }
    } else if (is_asearray || JS_IsArrayObject(cx, o)) {
      DBG(fprintf(stderr, "tosv: array object\n"));
      sv = pmc_create_jsproxy_wrap(aTHX_ ppl, cx, o, "JavaScript::AWrap", 0);
      if (!sv) {
	/* already thrown */
	return NULL;
      }
    } else {
      DBG(fprintf(stderr, "tosv: unknown object\n"));
      sv = pmc_create_jsproxy_wrap(aTHX_ ppl, cx, o, "JavaScript::HTWrap", 0);
      if (!sv) {
	/* already thrown */
	return NULL;
      }
    }
  }
  if (!sv) {
    DBG(pmc_dump_jsval(cx, "tosv", jv));
    sv = newSV(0);
  }
  return sv;
}

JSBool
pmc_sv_to_double(JSContext *cx, double dv, jsval *rval)
{
  jsdouble *dp;
  dp = JS_NewDouble(cx, dv);
  if (!dp) {
    (*rval) = JSVAL_VOID;
    return JS_FALSE;
  }
  (*rval) = DOUBLE_TO_JSVAL(dp);
  return JS_TRUE;
}

JSBool
pmc_sv_to_int(JSContext *cx, IV iv, jsval *rval)
{
  IV ivx;
  jsint v = iv;
  ivx = v;
  if (ivx == iv && INT_FITS_IN_JSVAL(v)) {
    DBG_INT(fprintf(stderr, "fits in jsval %d sizeof(IV)=%d\n", 
      v, (int)sizeof(IV)));
    (*rval) = INT_TO_JSVAL(v);
    return JS_TRUE;
  }
  if (ivx != iv) {
    DBG_INT(fprintf(stderr, "int overflow from iv\n"));
  }
  DBG_INT(fprintf(stderr, "int overflow jsval %d sizeof(IV)=%d\n", 
    v, (int)sizeof(IV)));
  return pmc_sv_to_double(cx, iv, rval);
}

JSBool
pmc_sv_to_string(pTHX_ JSContext *cx, pmc_tojs_mode mode,
  SV *sv /* must SvPOK */, jsval *rval)
{
  char *s = NULL;
  STRLEN len = 0;
  JSString *str;
  int is_utf = 0;

  s = SvPV(sv, len);
  if (mode == pmc_tojs_mode_auto) {
    is_utf = SvUTF8(sv);
    DBG_AUTO(fprintf(stderr, "AUTO: %d\n", is_utf ? 1 : 0));
  } else {
    is_utf = (mode == pmc_tojs_mode_text);
    DBG_AUTO(fprintf(stderr, "FORCE: %d\n", is_utf ? 1 : 0));
  }
  if (is_utf) {
    JSBool suc;
    suc = pmc_utf_jsval_setstring(cx, s, len, rval);
    return suc;
  } else {
    DBG_UTF(fprintf(stderr, "svtojs noutf\n"));
    str = JS_NewStringCopyN(cx, s, len);
  }
  if (!str) {
    (*rval) = JSVAL_VOID;
    return JS_FALSE;
  }
  (*rval) = STRING_TO_JSVAL(str);
  return JS_TRUE;
}

JSBool
pmc_plproxy_attach_plpriv_internal(pTHX_ PMCPerl *ppl, JSContext *cx,
  JSObject *obj, SV *sv, pmc_plpriv_calltype ct, const char *pkgname,
  const char *funcname)
{
  pmc_plpriv *pp;
  pmc_listnode *nd = NULL;
  if (!ppl) {
    pmc_throw_js_logic_error(cx, "InternalError", "pmc_plproxy_attach_plpriv",
      "perl pmcperl not found");
    return JS_FALSE;
  }
  #ifndef NDEBUG
  if (JS_GetPrivate(cx, obj)) {
    pmc_throw_js_logic_error(cx, "InternalError", "pmc_plproxy_attach_plpriv",
      "perl initpriv cantoverride");
    return JS_FALSE;
  }
  #endif
  pp = pmc_plpriv_new(ppl, cx);
  if (pp == NULL) {
    return JS_FALSE;
  }
  nd = pmc_register_perl_proxy(ppl, cx, obj);
  if (!nd) {
    goto err;
  }
  if (!JS_SetPrivate(cx, obj, pp)) {
    goto err;
  }
  if (sv) {
    pmc_plpriv_set_sv(pp, sv);
  }
  pmc_plpriv_set_calltype(pp, ct);
  if (pkgname) {
    if (!pmc_plpriv_set_package(pp, cx, pkgname)) {
      goto err;
    }
  }
  if (funcname) {
    if (!pmc_plpriv_set_function(pp, cx, funcname)) {
      goto err;
    }
  }
  pmc_plpriv_set_listnode(pp, nd);
  return JS_TRUE;

 err:
  if (nd) {
    pmc_unregister_perl_proxy(ppl, nd);
  }
  pmc_plpriv_destroy(pp);
  return JS_FALSE;

}

JSBool
pmc_plproxy_attach_plpriv(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  SV *sv)
{
  return pmc_plproxy_attach_plpriv_internal(aTHX_ ppl, cx, obj, sv,
    pmc_plpriv_calltype_none, NULL, NULL);
}

void
pmc_plproxy_detach_plpriv_moveto_finalized_list_noperllock(pTHX_ PMCPerl *ppl,
  JSContext *cx, JSObject *obj)
{
  /* NOTE: this function is called from plproxy finalizer without locking
    ppl->monitor. we must not call any perl api function. */
  pmc_plpriv *pp;
  pp = pmc_plproxy_get_plpriv(cx, obj);
  if (pp) {
    pmc_listnode *nd;
    nd = pmc_plpriv_get_listnode(pp);
    if (ppl && nd) {
      pmc_moveto_finalized_list_noperllock(ppl, cx, nd, pp);
      /* pp ownership is moved to pmo_finalized_list. it will be destroyed by 
	pmc_clean_finalized() */
    }
  }
}

void
pmc_plproxy_detach_plpriv_internal(pTHX_ PMCPerl *ppl, JSContext *cx,
  JSObject *obj, int lock_flag)
{
  pmc_plpriv *pp;
  pp = pmc_plproxy_get_plpriv(cx, obj);
  if (pp) {
    pmc_listnode *nd;
    nd = pmc_plpriv_get_listnode(pp);
    if (ppl && nd) {
      if (lock_flag) {
	pmc_unregister_perl_proxy(ppl, nd);
      } else {
	pmc_unregister_perl_proxy_nolock(ppl, nd);
      }
    }
    pmc_plpriv_destroy(pp);
    DBG_PRIV(fprintf(stderr, "__fpriv %p %p\n", obj, pp));
    JS_SetPrivate(cx, obj, NULL);
    JS_ClearScope(cx, obj);
  }
}

void
pmc_plproxy_detach_plpriv(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj)
{
  pmc_plproxy_detach_plpriv_internal(aTHX_ ppl, cx, obj, 1);
}

void
pmc_plproxy_detach_plpriv_nolock(pTHX_ PMCPerl *ppl, JSContext *cx,
  JSObject *obj)
{
  pmc_plproxy_detach_plpriv_internal(aTHX_ ppl, cx, obj, 0);
}

static JSObject *
pmc_sv_extract_jsobj(pTHX_ JSContext *cx, SV *sv)
{
  SV *svet;
  pmc_jsobject *rep;

  svet = SvRV(sv);
  if (!svet) {
    DBG_WRAP(fprintf(stderr, "extract !svet\n"));
    return NULL;
  }
  if (!SvIOK(svet)) {
    DBG_WRAP(fprintf(stderr, "extract !SvIOK(svet)\n"));
    return NULL;
  }
  rep = (pmc_jsobject *)SvIV(svet);
  if (!rep) {
    DBG_WRAP(fprintf(stderr, "extract !rep\n"));
    return NULL;
  }
  return pmc_jsobject_get_js(rep);
}

static const char *
pmc_get_klassname(pTHX_ SV *svtarget)
{
  if (!svtarget || !SvOBJECT(svtarget)) {
    return NULL;
  }
  return HvNAME(SvSTASH(svtarget));
}

static int
pmc_sv_isa(pTHX_ SV *sv, const char *name)
{
  SV *svtarget;
  const char *stname;
  if (!sv || !SvROK(sv)) {
    return 0;
  }
  svtarget = SvRV(sv);
  stname = pmc_get_klassname(aTHX_ svtarget);
  return (stname && strcmp(stname, name) == 0);
}

static JSObject *
pmc_sv_extract_htawrap(pTHX_ JSContext *cx, SV *sv)
{
  /* sv should be a hashref or arrayref */
  SV *svtarget, *tied, *elem;
  JSObject *obj = NULL;
  MAGIC *mg = NULL;

  if (!SvROK(sv)) {
    DBG_WRAP(fprintf(stderr, "extract !SvROK(sv)\n"));
    goto out;
  }
  svtarget = SvRV(sv);
  if (!svtarget) {
    DBG_WRAP(fprintf(stderr, "extract !svtarget\n"));
    goto out;
  }
  if (!SvMAGICAL(svtarget)) {
    DBG_WRAP(fprintf(stderr, "extract !magical\n"));
    goto out;
  }
  mg = mg_find(svtarget, PERL_MAGIC_tied);
  if (!mg) {
    DBG_WRAP(fprintf(stderr, "extract tied magic not found\n"));
    goto out;
  }
  tied = mg->mg_obj; /* 'tied %$instance' */
  if (!tied) {
    DBG_WRAP(fprintf(stderr, "extract !tied\n"));
    goto out;
  }
  if (!SvROK(tied)) {
    DBG_WRAP(fprintf(stderr, "extract !SvROK(tied)\n"));
    goto out;
  }
  elem = SvRV(tied);
  if (!SvROK(elem)) {
    DBG_WRAP(fprintf(stderr, "extract !SvROK(elem)\n"));
    goto out;
  }
  if (!pmc_sv_isa(aTHX_ elem, "JavaScript")) {
    DBG_WRAP(fprintf(stderr, "extract !sv_isa(JavaScript)\n"));
    goto out;
  }
  DBG_WRAP(fprintf(stderr, "extract OK\n"));
  obj = pmc_sv_extract_jsobj(aTHX_ cx, elem);

 out:

  return obj;
}

static JSObject *
pmc_sv_extract_jsfunc_from_cv(pTHX_ JSContext *cx, CV *cv, CV *outercv)
{
  JSBool suc = JS_TRUE;
  SV *arg = NULL;
  int rvalc = 0;
  SV **rvalv = NULL;
  SV *r = NULL;
  JSObject *obj = NULL;

  DBG_EXTRACT(fprintf(stderr, "extract jsfunc\n"));
  arg = newRV_inc((SV *)outercv);
  suc = pmc_funccall_sv_internal(aTHX_ cx, 1, &arg, &rvalc, &rvalv, (SV *)cv,
    G_SCALAR);
  if (suc) {
    DBG_EXTRACT(fprintf(stderr, "extract jsfunc: call success\n"));
    if (rvalc > 0) {
      r = rvalv[0];
      if (r && SvROK(r) && pmc_sv_isa(aTHX_ r, "JavaScript")) {
	DBG_EXTRACT(fprintf(stderr, "extract jsfunc: yes its a js\n"));
	obj = pmc_sv_extract_jsobj(aTHX_ cx, r);
      } else {
	DBG_EXTRACT(fprintf(stderr, "extract jsfunc: throw\n"));
	pmc_throw_js_logic_error(cx, "InternalError",
	  "pmc_sv_extract_jsfunc_from_cv", "not a js object");
      }
    } else {
      pmc_throw_js_logic_error(cx, "InternalError",
	"pmc_sv_extract_jsfunc_from_cv", "retval");
      DBG_EXTRACT(fprintf(stderr, "extract jsfunc: insane\n"));
    }
  }
  SvREFCNT_dec(arg);
  pmc_free_svarr(aTHX_ rvalc, rvalv, "pmc_create_func_wrap");
  return obj;
}

static JSBool
pmc_create_nomagic_sv(pTHX_ PMCPerl *ppl, JSContext *cx, SV *svarg, SV **svr)
{
  /* tied scalar, hash etc. */
  if (SvMAGICAL(svarg)) {
    JSBool suc = JS_TRUE;
    SV *argv[2];
    int rvalc = 0;
    SV **rvalv = NULL;
    DBG_SETSV(fprintf(stderr, "pmc_create_nomagic_sv: setsv magic\n"));

    /* require JavaScript::Util */
    pmc_load_javascript_util(ppl, cx);

    (*svr) = newSV(0);
    argv[0] = (*svr);
    argv[1] = svarg;
    suc = pmc_funccall_internal(aTHX_ cx, 2, argv, &rvalc, &rvalv,
      "JavaScript::Util::sv_setsv", 0, G_SCALAR);
    pmc_free_svarr(aTHX_ rvalc, rvalv, "pmc_setsv_internal");
    return suc;
  } else {
    SvREFCNT_inc(svarg);
    (*svr) = svarg;
    return JS_TRUE;
  }
}

static CV *
pmc_sv_is_jsfunc(pTHX_ SV *sv, CV **cvouter_r)
{
  SV *svtarget;
  CV *cv, *cvouter;
  GV *gv = NULL;
  HV *stash = NULL;
  const char *stash_name;
  const char *gv_name;
  STRLEN gv_namelen;

  (*cvouter_r) = NULL;
  if (!SvROK(sv)) {
    return NULL;
  }
  svtarget = SvRV(sv);
  if (!svtarget) {
    return NULL;
  }
  if (SvTYPE(svtarget) != SVt_PVCV) {
    return NULL;
  }
  /* check whether this cv is a wrapped JSFunction made by
    JavaScript::CWrap::new */
  cv = (CV *)svtarget;
  cvouter = CvOUTSIDE(cv);
  if (!cvouter) {
    return NULL;
  }
  gv = CvGV(cvouter);
  if (!gv) {
    return NULL;
  }
  stash = GvSTASH(gv);
  stash_name = HvNAME(stash);
  gv_name = GvNAME(gv);
  gv_namelen = GvNAMELEN(gv);
  if (gv_namelen != 3 || memcmp(gv_name, "new", 3) != 0 ||
    strcmp(stash_name, "JavaScript::CWrap") != 0) {
    return NULL;
  }
  (*cvouter_r) = cvouter;
  return cv;
}

static JSBool
pmc_sv_to_jsval_internal(pTHX_ PMCPerl *ppl, JSContext *cx, pmc_tojs_mode mode,
  SV *svarg, jsval *rval)
{
  JSBool suc = JS_TRUE;
  SV *sv = NULL;
  int noconv = 0;

  DBG(fprintf(stderr, "tojs: svarg=%p rval=%p\n", svarg, rval));
  if (!svarg) {
    DBG(fprintf(stderr, "tojs: void\n"));
    (*rval) = JSVAL_VOID;
    return JS_TRUE;
  }
  if (!pmc_create_nomagic_sv(aTHX_ ppl, cx, svarg, &sv) || sv == NULL) {
    DBG(fprintf(stderr, "tojs: tomagic: void\n"));
    (*rval) = JSVAL_VOID;
    return JS_TRUE;
  }

  noconv = (mode == pmc_tojs_mode_keep);
  DBG(fprintf(stderr, "tojs: noconv=%d\n", (int)noconv));

  if (SvIOKp(sv)) {
    suc = pmc_sv_to_int(cx, SvIV(sv), rval);
    DBG(fprintf(stderr, "tojs: int raddr=%p tag=%d\n", rval,
      (int)JSVAL_TAG(*rval)));
    goto out;
  } else if (SvNOKp(sv)) {
    DBG(fprintf(stderr, "tojs: double\n"));
    suc = pmc_sv_to_double(cx, SvNV(sv), rval);
    goto out;
  } else if (SvPOKp(sv)) {
    DBG(fprintf(stderr, "tojs: str\n"));
    if (!noconv) {
      suc = pmc_sv_to_string(aTHX_ cx, mode, sv, rval);
      goto out;
    }
    /* thru */
  } else if (SvROK(sv)) {
    const char *klassname;
    JSObject *obj;
    klassname = pmc_get_klassname(aTHX_ SvRV(sv));
    obj = pmc_sv_extract_htawrap(aTHX_ cx, sv);
    DBG(fprintf(stderr, "tojs: reftype\n"));
    if (obj != NULL) {
      DBG(fprintf(stderr, "tojs: extracted htawrap object\n"));
      (*rval) = OBJECT_TO_JSVAL(obj);
      suc = JS_TRUE;
      goto out;
    } else if (klassname == NULL) {
      /* not an object */
      CV *cv = NULL;
      CV *cvouter = NULL;
      DBG(fprintf(stderr, "tojs: is not an object\n"));
      cv = pmc_sv_is_jsfunc(aTHX_ sv, &cvouter);
      if (cv && cvouter) {
	/* non-blessed wrapped js function */
	JSObject *obj;
	obj = pmc_sv_extract_jsfunc_from_cv(aTHX_ cx, cv, cvouter);
	if (!obj) {
	  /* already thrown */
	  (*rval) = JSVAL_VOID;
	  suc = JS_FALSE;
	  goto out;
	}
	(*rval) = OBJECT_TO_JSVAL(obj);
	suc = JS_TRUE;
	goto out;
      }
      /* thru */
    } else if (strcmp(klassname, "JavaScript") == 0) {
      /* not wrapped js object. */
      DBG(fprintf(stderr, "tojs: is a not-wrapped js object\n"));
      obj = pmc_sv_extract_jsobj(aTHX_ cx, sv);
      if (!obj) {
	DBG(fprintf(stderr,
	  "tojs: failed to extract not-wrapped js object\n"));
	(*rval) = JSVAL_VOID;
      } else {
	DBG_WRAP(fprintf(stderr, "tojs: extracted not-wrapped js object\n"));
	(*rval) = OBJECT_TO_JSVAL(obj);
      }
      suc = JS_TRUE;
      goto out;
    }
  } else if (!SvTRUE(sv)) {
    DBG(fprintf(stderr, "tojs: void\n"));
    (*rval) = JSVAL_VOID;
    suc = JS_TRUE;
    goto out;
  } 
  /* cant convert to simple value. create Perl proxy object. */
  DBG(fprintf(stderr, "tojs: perproxy\n"));
  suc = pmc_sv_to_wrappedsv(aTHX_ ppl, cx, sv, rval);

 out:

  SvREFCNT_dec(sv);
  DBG(fprintf(stderr, "tojs-done\n"));
  return suc;
}


JSBool
pmc_sv_to_wrappedsv(pTHX_ PMCPerl *ppl, JSContext *cx, SV *sv, jsval *rval)
{
  JSClass *cl = &pmc_perl_class;
  JSObject *obj;
  DBG(fprintf(stderr, "tojs: pobj\n"));
  obj = JS_NewObject(cx, cl, NULL, NULL);
  if (obj == NULL) {
    (*rval) = JSVAL_VOID;
    return JS_FALSE;
  }
  (*rval) = OBJECT_TO_JSVAL(obj);
  if (!pmc_plproxy_attach_plpriv(aTHX_ ppl, cx, obj, sv)) {
    (*rval) = JSVAL_VOID;
    return JS_FALSE;
  }
  return JS_TRUE;
}

JSBool
pmc_sv_to_jsval(pTHX_ PMCPerl *ppl, JSContext *cx, pmc_tojs_mode mode,
  SV *svarg, jsval *rval)
{
  JSBool suc;
  suc = pmc_sv_to_jsval_internal(aTHX_ ppl, cx, mode, svarg, rval);
  return suc;
}

JSBool
pmc_sv_to_jsval_array(pTHX_ PMCPerl *ppl, JSContext *cx, pmc_tojs_mode mode,
  int rvalc, SV **rvalv, jsval *rval)
{
  int i;
  JSObject *arr;
  JSBool suc = JS_TRUE;

  if (rvalc == 0) {
    /* empty */
    (*rval) = JSVAL_VOID;
    return JS_TRUE;
  }
  if (!JS_AddNamedRoot(cx, &arr, "pmc_sv_to_jsval_array")) {
    (*rval) = JSVAL_VOID;
    return JS_FALSE;
  }
  arr = JS_NewArrayObject(cx, rvalc, NULL);
  if (arr == NULL) {
    (*rval) = JSVAL_VOID;
    suc = JS_FALSE;
    goto out;
  }
  (*rval) = OBJECT_TO_JSVAL(arr);
  for (i = 0; i < rvalc; ++i) {
    jsval ev = JSVAL_VOID;
    if (!pmc_sv_to_jsval(aTHX_ ppl, cx, mode, rvalv[i], &ev)) {
      (*rval) = JSVAL_VOID;
      suc = JS_FALSE;
      goto out;
    }
    if (!JS_SetElement(cx, arr, rvalc - i - 1, &ev)) {
      (*rval) = JSVAL_VOID;
      suc = JS_FALSE;
      goto out;
    }
  }

 out:
  JS_RemoveRoot(cx, &arr);
  return suc;
}

JSBool
pmc_plproxy_new_pkgobject_stash(pTHX_ PMCPerl *ppl, JSContext *cx,
  pmc_plpriv_calltype ct, const char *pkgname, jsval *rval)
{
  JSObject *obj = NULL;
  JSBool suc = JS_FALSE;
  HV *stash = NULL;
  SV *entsv;
  char *key = NULL;
  I32 klen = 0;
  DBG_PKG(fprintf(stderr, "newpkgobjstash %s\n", pkgname));
  if (!JS_AddNamedRoot(cx, &obj, "pmc_plproxy_new_pkgobject_stash")) {
    return JS_FALSE;
  }
  obj = JS_NewObject(cx, NULL, NULL, NULL);
  if (obj == NULL) {
    goto out;
  }
  (*rval) = OBJECT_TO_JSVAL(obj);
  stash = gv_stashpv(pkgname, 0);
  if (!stash || !SvREFCNT(stash)) {
    pmc_throw_js_logic_error(cx, "PackageNotFound",
      "pmc_plproxy_new_pkgobject_stash", "package not found");
    goto out;
  }
  hv_iterinit(stash);
  while ((entsv = hv_iternextsv(stash, &key, &klen)) != NULL) {
    GV *gv;
    CV *cv;
    SV *cref;
    jsval jv = JSVAL_VOID;
    if (!entsv) {
      continue;
    }
    gv = (GV *)entsv;
    cv = GvCV(gv);
    if (!cv) {
      continue;
    }
    cref = newRV_inc((SV *)cv);
    suc = pmc_sv_to_wrappedsv(aTHX_ ppl, cx, cref, &jv);
    SvREFCNT_dec(cref);
    if (!suc) {
      goto out;
    }
    suc = pmc_utf_defineproperty(cx, obj, key, klen, 1, jv, NULL, NULL, 0);
    if (!suc) {
      goto out;
    }
  }

 out:

  if (stash) {
    hv_iterinit(stash);
  }
  JS_RemoveRoot(cx, &obj);
  if (!suc) {
    (*rval) = JSVAL_VOID;
  }
  return suc;
}

JSBool
pmc_plproxy_new_pkgobject(pTHX_ PMCPerl *ppl, JSContext *cx,
  pmc_plpriv_calltype ct, const char *pkgname, const char *funcname,
  jsval *rval)
{
  JSObject *obj;
  DBG_PKG(fprintf(stderr, "newpkgobj %s\n", pkgname));
  obj = JS_NewObject(cx, &pmc_perl_class, NULL, NULL);
  if (obj == NULL) {
    (*rval) = JSVAL_VOID;
    return JS_FALSE;
  }
  (*rval) = OBJECT_TO_JSVAL(obj);
  if (!pmc_plproxy_attach_plpriv_internal(aTHX_ ppl, cx, obj, NULL, ct,
    pkgname, funcname)) {
    (*rval) = JSVAL_VOID;
    return JS_FALSE;
  }
  return JS_TRUE;
}

JSBool
pmc_plproxy_new_methodsv(pTHX_ PMCPerl *ppl, JSContext *cx, SV *sv,
  JSObject *parent, jsval *rval)
{
  JSObject *obj;
  obj = JS_NewObject(cx, &pmc_perl_class, NULL, parent);
  if (obj == NULL) {
    (*rval) = JSVAL_VOID;
    return JS_FALSE;
  }
  (*rval) = OBJECT_TO_JSVAL(obj);
  if (!pmc_plproxy_attach_plpriv_internal(aTHX_ ppl, cx, obj, sv,
    pmc_plpriv_calltype_methodsv, NULL, NULL)) {
    (*rval) = JSVAL_VOID;
    return JS_FALSE;
  }
  return JS_TRUE;
}

JSBool
pmc_plproxy_getvar_internal(pTHX_ PMCPerl *ppl, JSContext *cx,
  const char *name, jsval *vp)
{
  SV *sv;
  sv = get_sv(name, 0);
  return pmc_sv_to_jsval(aTHX_ ppl, cx,
    pmc_plproxy_get_tojs_current_mode(ppl), sv, vp);
}

JSBool
pmc_plproxy_setvar_internal(pTHX_ PMCPerl *ppl, JSContext *cx,
  const char *name, jsval *vp)
{
  SV *sv, *valsv;
  sv = get_sv(name, 1);
  valsv = pmc_jsval_to_sv(aTHX_ ppl, cx, (*vp), 0);
  if (!valsv) {
    return JS_FALSE;
  }
  if (sv && sv != valsv) {
    sv_setsv(sv, valsv);
  }
  SvREFCNT_dec(valsv);
  return JS_TRUE;
}

pmc_tojs_mode
pmc_plproxy_get_tojs_current_mode(PMCPerl *ppl)
{
  return pmc_get_suppress_conv(ppl) ? pmc_tojs_mode_keep : pmc_tojs_mode_auto;
}


