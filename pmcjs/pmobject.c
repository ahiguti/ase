
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include "pmobject.h"
#include "poprivate.h"
#include "utfutil.h"
#include "pjsobject_js.h"
#include "pmconv.h"
#include "pmutf.h"

#include <jsfun.h>

#define DBG(x)
#define DBG_INT(x)
#define DBG_ARR(x)
#define DBG_LEN(x)
#define DBG_READONLY(x)
#define DBG_FETCH(x)
#define DBG_ARRCTX(x)
#define DBG_UTFERR(x)
#define DBG_UTF(x)
#define DBG_CONV(x)
#define DBG_GETPROP(x)
#define DBG_SETPROP(x)
#define DBG_DELPROP(x)
#define DBG_ENUM(x)
#define DBG_CJS(x)
#define DBG_USE(x)
#define DBG_TOSTRING(x)
#define DBG_VAR(x)
#define DBG_ENABLE_LEAK_DUMMY(x)
#define DBG_CALLSV(x)
#define DBG_TIED(x)
#define DBG_FETCHSIZE(x)
#define DBG_SETSV(x)
#define DBG_TIECNT(x)
#define DBG_NOMETHOD(x)
#define DBG_FIN(x)
#define DBG_METHODCACHE(x)
#define DBG_MOD(x)
#define DBG_FUNCOBJ(x)
#define DBG_PROTO(x)

#define PMC_ALLOC_SVARR(cx, cnt, id) (SV **) \
  ((cnt < 32) ? alloca(cnt * sizeof(SV *)) \
    : pmc_malloc(cx, cnt * sizeof(SV *), id))

#define PMC_ALLOCFREE_SVARR(cnt, svarr, id) \
{ \
  if (cnt >= 32) { \
    pmc_free_svarr(aTHX_ cnt, svarr, id); \
  } else { \
    pmc_free_svarr_contentsonly(aTHX_ cnt, svarr); \
  } \
}

static const char *pmc_plproxy_methods_for_object[9];
static JSFunctionSpec pmc_plproxy_methods[15];

static PMCPerl *
pmc_plproxy_get_pmcperl(JSContext *cx, JSObject *obj)
{
  pmc_plpriv *pp = NULL;
  PMCPerl *ppl = NULL;
  jsval protojv = JSVAL_VOID;
  JSObject *protoobj = NULL;

  pp = pmc_plproxy_get_plpriv(cx, obj);
  ppl = pmc_plpriv_get_pmcperl(pp);
  if (ppl) {
    return ppl;
  }
  DBG_PROTO(fprintf(stderr, "PROTO: nopriv klass=%s\n",
    JS_GetClass(cx, obj)->name));
  if (!JS_ObjectIsFunction(cx, obj)) {
    DBG_PROTO(fprintf(stderr, "PROTO: not function\n"));
    protoobj = JS_GetPrototype(cx, obj);
  } else {
    if (!JS_GetProperty(cx, obj, "prototype", &protojv)) {
      DBG_PROTO(fprintf(stderr, "PROTO: no prototype\n"));
      return NULL;
    }
    if (!JSVAL_IS_OBJECT(protojv)) {
      return NULL;
    }
    protoobj = JSVAL_TO_OBJECT(protojv);
  }
  if (!protoobj) {
    return NULL;
  }
  pp = pmc_plproxy_get_plpriv(cx, protoobj);
  return pmc_plpriv_get_pmcperl(pp);
}

static PMCPerl *
pmc_plproxy_enter(JSContext *cx, JSObject *obj, JSContext **oldcx_r)
{
  PMCPerl *ppl;
  ppl = pmc_plproxy_get_pmcperl(cx, obj);
  if (!ppl) {
    (*oldcx_r) = NULL;
    return ppl;
  }
  (*oldcx_r) = pmc_enter_pmcperl(ppl, cx);
  return ppl;
}

static void
pmc_plproxy_leave(PMCPerl *ppl, JSContext *oldcx)
{
  if (ppl) {
    pmc_leave_pmcperl(ppl, oldcx);
  }
}

static JSBool
pmc_funccall_svorpv_argconv(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, int *rvalcp, SV ***rvalvp, SV *funcsv,
  const char *funcname, int is_method, I32 evalctx)
{
  SV **svargv;
  uintN i;
  JSBool suc = JS_TRUE;

  svargv = (SV **)pmc_malloc(cx, sizeof(*svargv) * argc,
    "pmc_funccall_argconv");
  //svargv = PMC_ALLOC_SVARR(cx, argc, "pmc_funccall_argconv");
  if (!svargv) {
    return JS_FALSE;
  }
  for (i = 0; i < argc; ++i) {
    svargv[i] = pmc_jsval_to_sv(aTHX_ ppl, cx, argv[i], 0);
    if (svargv[i] == NULL) {
      suc = JS_FALSE;
    }
  }
  if (suc) {
    DBG(fprintf(stderr, "perl call: argc = %d\n", argc));
    suc = pmc_funccall_svorpv_internal(aTHX_ cx, argc, svargv, rvalcp, rvalvp,
      funcsv, funcname, is_method, evalctx);
  }
  pmc_free_svarr(aTHX_ argc, svargv, "pmc_funccall_argconv");
  // PMC_ALLOCFREE_SVARR(argc, svargv, "pmc_funccall_argconv");
  return suc;
}

static JSBool
pmc_funccall_argconv(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, int *rvalcp, SV ***rvalvp, const char *funcname,
  int is_method, I32 evalctx)
{
  return pmc_funccall_svorpv_argconv(aTHX_ ppl, cx, obj, argc, argv, rvalcp,
    rvalvp, NULL, funcname, is_method, evalctx);
}

static JSBool
pmc_funccall_sv_argconv(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, int *rvalcp, SV ***rvalvp, SV *funcsv, I32 evalctx)
{
  return pmc_funccall_svorpv_argconv(aTHX_ ppl, cx, obj, argc, argv, rvalcp,
    rvalvp, funcsv, "", 0, evalctx);
}

static JSBool
pmc_funccall_sv_scalarctx(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval, SV *funcsv)
{
  SV **rvalv = NULL;
  int rvalc = 0;
  JSBool suc = JS_FALSE;
  suc = pmc_funccall_sv_argconv(aTHX_ ppl, cx, obj, argc, argv, &rvalc,
    &rvalv, funcsv, G_SCALAR);
  if (suc) {
    if (rvalc > 0) {
      suc = pmc_sv_to_jsval(aTHX_ ppl, cx,
	pmc_plproxy_get_tojs_current_mode(ppl), rvalv[0], rval);
    } else {
      pmc_throw_js_runtime_exception(cx, "InternalError",
	"pmc_funccall_sv_scalarctx", "no retval");
      suc = JS_FALSE;
    }
  }
  pmc_free_svarr(aTHX_ rvalc, rvalv, "pmc_funccall_sv_scalarctx");
  return suc;
}

static JSBool
pmc_funccall_scalarctx(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval, const char *funcname,
  int is_method)
{
  SV **rvalv = NULL;
  int rvalc = 0;
  JSBool suc = JS_FALSE;
  suc = pmc_funccall_svorpv_argconv(aTHX_ ppl, cx, obj, argc, argv, &rvalc,
    &rvalv, NULL, funcname, is_method, G_SCALAR);
  if (suc) {
    if (rvalc > 0) {
      suc = pmc_sv_to_jsval(aTHX_ ppl, cx,
	pmc_plproxy_get_tojs_current_mode(ppl), rvalv[0], rval);
    } else {
      pmc_throw_js_runtime_exception(cx, "InternalError",
	"pmc_funccall_scalarctx", "no retval");
      suc = JS_FALSE;
    }
  }
  pmc_free_svarr(aTHX_ rvalc, rvalv, "pmc_funccall_scalarctx");
  return suc;
}

static JSBool
pmc_plproxy_use_internal(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  char *namestr /* must be writable and nul-terminated */, size_t u8len)
{
  /* not used */
  JSBool suc = JS_TRUE;
  return suc;
}

static JSBool
pmc_plproxy_use_byjv(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval name)
{
  JSBool suc = JS_TRUE;
  char *namestr = NULL;
  size_t u8len = 0;
  namestr = pmc_utf_jsval_getstring(cx, name, &u8len, "pmc_plproxy_use_jv");
  if (!namestr) {
    return JS_FALSE;
  }
  suc = pmc_plproxy_use_internal(aTHX_ ppl, cx, obj, namestr, u8len);
  pmc_free(namestr, "pmc_plproxy_use_jv");
  return suc;
}

static JSBool
pmc_plproxy_use_bystr(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  const char *name)
{
  JSBool suc = JS_TRUE;
  char *namestr = NULL;
  size_t u8len = 0;
  namestr = pmc_strdup(cx, name, "pmc_plproxy_use_str");
  if (!namestr) {
    return JS_FALSE;
  }
  u8len = strlen(namestr);
  suc = pmc_plproxy_use_internal(aTHX_ ppl, cx, obj, namestr, u8len);
  pmc_free(namestr, "pmc_plproxy_use_bystr");
  return suc;
}

static JSBool
pmc_plproxy_use(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj, uintN argc,
  jsval *argv, jsval *rval)
{
  JSBool suc;
  int rvalc = 0;
  SV **rvalv = NULL;
  if (argc < 1) {
    return pmc_throw_js_logic_error(cx, "MissingArgs", "pmc_plproxy_use",
      "missing arg(s)");
  }
  {
    DBG_PROTO(jsval protoval = JSVAL_VOID);
    DBG_PROTO(JSFunction *f = JS_ValueToFunction(cx, OBJECT_TO_JSVAL(obj)));
    DBG_PROTO(JS_GetProperty(cx, obj, "prototype", &protoval));
    DBG_PROTO(fprintf(stderr, "USE KLASS=%p\n", f->clasp));
    DBG_PROTO(fprintf(stderr, "USE PROTO=%p KLASS=%s\n",
      JSVAL_TO_OBJECT(protoval),
      JS_GetClass(cx, JSVAL_TO_OBJECT(protoval))->name));
  }
  suc = pmc_funccall_argconv(aTHX_ ppl, cx, obj, argc, argv, &rvalc, &rvalv,
    "JavaScript::LoadModule", 0, G_SCALAR);
  pmc_free_svarr(aTHX_ rvalc, rvalv, "pmc_plproxy_use");
  return suc;
}

static JSBool
pmc_plproxy_constr(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  int rvalc;
  SV **rvalv;
  JSBool suc = JS_TRUE;
  const int is_method = 1;

  DBG(fprintf(stderr, "pmc_plproxy_constr\n"));
  rvalc = 0;
  rvalv = NULL;
  (*rval) = OBJECT_TO_JSVAL(obj);

  if (argc > 0) {
    if (!pmc_plproxy_use_byjv(aTHX_ ppl, cx, obj, argv[0])) {
      (*rval) = JSVAL_VOID;
      return JS_FALSE;
    }
    DBG(fprintf(stderr, "pmc_plproxy_constr : new\n"));
    suc = pmc_funccall_argconv(aTHX_ ppl, cx, obj, argc, argv, &rvalc, &rvalv,
      "new", is_method, G_SCALAR);
    if (suc) {
      if (rvalc == 1 && rvalv[0] != NULL) {
	SV *nsv = rvalv[0];
	if (SvTRUE(nsv)) {
	  suc = pmc_plproxy_attach_plpriv(aTHX_ ppl, cx, obj, nsv);
	} else {
	  pmc_throw_js_runtime_exception(cx, "PerlNewFailed",
	    "pmc_plproxy_constr", "new returns null");
	  suc = JS_FALSE;
	}
      } else {
	pmc_throw_js_runtime_exception(cx, "PerlNewFailed",
	  "pmc_plproxy_constr", "new method failed");
	suc = JS_FALSE;
      }
    }
    pmc_free_svarr(aTHX_ rvalc, rvalv, "pmc_plproxy_constr");
    DBG(fprintf(stderr, "pmc_plproxy_constr: %d\n", suc));
  } else {
    suc = pmc_throw_js_logic_error(cx, "MissingArgs", "pmc_plproxy_constr",
      "missing arg(s)");
  }
  if (!suc) {
    (*rval) = JSVAL_VOID;
  }
  return suc;
}

static void
pmc_plproxy_finalize_noperllock(JSContext *cx, JSObject *obj)
{
  /* NOTE: we don't lock ppl->monitor */
  PMCPerl *ppl;
  PerlInterpreter *my_perl;
  DBG_FIN(fprintf(stderr, "pmc_plproxy_finalize %p\n", obj));
  ppl = pmc_plproxy_get_pmcperl(cx, obj);
  my_perl = pmc_get_interpreter(ppl);
  if (!my_perl) {
    return;
  }
  pmc_plproxy_detach_plpriv_moveto_finalized_list_noperllock(my_perl, ppl, cx,
    obj);
}

// FIXME: remove
#if 0
static void
pmc_plproxy_finalize(JSContext *cx, JSObject *obj)
{
  PMCPerl *ppl;
  PerlInterpreter *my_perl;
  JSContext *oldcx;
  DBG_FIN(fprintf(stderr, "pmc_plproxy_finalize %p\n", obj));
  ppl = pmc_plproxy_enter(cx, obj, &oldcx);
  my_perl = pmc_get_interpreter(ppl);
  if (!my_perl) {
    goto out;
  }
  pmc_plproxy_detach_plpriv(my_perl, ppl, cx, obj);
 out:
  pmc_plproxy_leave(ppl, oldcx);
}
#endif

static JSBool
pmc_plproxy_invoke_internal(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval, int is_method, int obj_as_firstarg,
  I32 evalctx)
{
  /* argv = [ "methodname", arg1, arg2, ... ] */
  /* NOTE: argv[] are not rooted, while their values are on rooted memory. */
  int rvalc;
  SV **rvalv;
  JSBool suc = JS_TRUE;
  char *u8name;
  size_t u8len = 0;

  DBG(fprintf(stderr, "pmc_plproxy_invoke\n"));
  rvalc = 0;
  rvalv = NULL;

  u8name = pmc_utf_jsval_getstring(cx, argv[0], &u8len, "pmc_plproxy_invoke");
  if (!u8name) {
    (*rval) = JSVAL_VOID;
    return JS_FALSE;
  }
  DBG(fprintf(stderr, "pmc_plproxy_invoke: [%s]\n", u8name));

  if (obj_as_firstarg) {
    argv[0] = OBJECT_TO_JSVAL(obj);
    suc = pmc_funccall_argconv(aTHX_ ppl, cx, obj, argc, argv, &rvalc, &rvalv,
      u8name, is_method, evalctx);
  } else {
    suc = pmc_funccall_argconv(aTHX_ ppl, cx, obj, argc - 1, argv + 1, &rvalc,
      &rvalv, u8name, is_method, evalctx);
  }
  pmc_free(u8name, "pmc_plproxy_invoke_internal");

  DBG(fprintf(stderr, "pmc_plproxy_invoke: %d\n", suc));
  if (suc) {
    if (evalctx == G_SCALAR) {
      if (rvalc > 0) {
	suc = pmc_sv_to_jsval(aTHX_ ppl, cx,
	  pmc_plproxy_get_tojs_current_mode(ppl), rvalv[0], rval);
      } else {
	(*rval) = JSVAL_VOID;
	suc = JS_TRUE;
      }
    } else {
      suc = pmc_sv_to_jsval_array(aTHX_ ppl, cx,
	pmc_plproxy_get_tojs_current_mode(ppl), rvalc, rvalv, rval);
    }
  } else {
    (*rval) = JSVAL_VOID;
  }
  pmc_free_svarr(aTHX_ rvalc, rvalv, "pmc_plproxy_invoke_internal");
  return suc;
}

static svtype
pmc_plproxy_get_svtype(pTHX_ pmc_plpriv *pp, int *is_object_r)
{
  SV *sv, *svtarget;
  (*is_object_r) = 0;
  sv = pmc_plpriv_get_sv(pp);
  if (!sv) {
    return SVt_NULL;
  }
  if (!SvROK(sv)) {
    return SVt_NULL;
  }
  svtarget = SvRV(sv);
  if (!svtarget) {
    return SVt_NULL;
  }
  (*is_object_r) = SvOBJECT(svtarget);
  return SvTYPE(svtarget);
}

static int
pmc_plproxy_is_object(pTHX_ JSContext *cx, JSObject *obj)
{
  pmc_plpriv *pp;
  svtype ty;
  int isobj = 0;
  pp = pmc_plproxy_get_plpriv(cx, obj);
  if (!pp) {
    return 0;
  }
  ty = pmc_plproxy_get_svtype(aTHX_ pp, &isobj);
  return isobj;
}

static JSBool
pmc_plproxy_invoke(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  const int is_method = 1;
  const int obj_as_firstarg = 1;

  if (!pmc_plproxy_is_object(aTHX_ cx, obj)) {
    (*rval) = JSVAL_VOID;
    return pmc_throw_js_logic_error(cx, "TypeMismatch", "pmc_plproxy_invoke",
      "not a perl object");
  }
  return pmc_plproxy_invoke_internal(aTHX_ ppl, cx, obj, argc, argv, rval,
    is_method, obj_as_firstarg, G_SCALAR);
}

static JSBool
pmc_plproxy_invokelc(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  const int is_method = 1;
  const int obj_as_firstarg = 1;

  if (!pmc_plproxy_is_object(aTHX_ cx, obj)) {
    (*rval) = JSVAL_VOID;
    return pmc_throw_js_logic_error(cx, "TypeMismatch", "pmc_plproxy_invokelc",
      "not a perl object");
  }
  if (argc < 1) {
    return pmc_throw_js_logic_error(cx, "MissingArgs", "pmc_plproxy_invokelc",
      "missing arg(s)");
  }
  return pmc_plproxy_invoke_internal(aTHX_ ppl, cx, obj, argc, argv, rval,
    is_method, obj_as_firstarg, G_ARRAY);
}

static JSBool
pmc_plproxy_newobject(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  obj = JS_NewObject(cx, &pmc_perl_class, NULL, NULL); /* overwrite obj */
  if (!obj) {
    (*rval) = JSVAL_VOID;
    return JS_FALSE;
  }
  return pmc_plproxy_constr(aTHX_ ppl, cx, obj, argc, argv, rval);
}

static JSBool
pmc_plproxy_newscalar_internal(pTHX_ PMCPerl *ppl, JSContext *cx,
  JSObject *obj, uintN argc, jsval *argv, jsval *rval, svtype svt)
{
  JSBool suc = JS_TRUE;

  obj = JS_NewObject(cx, &pmc_perl_class, NULL, NULL); /* overwrite obj */
  if (!obj) {
    (*rval) = JSVAL_VOID;
    return JS_FALSE;
  }
  (*rval) = OBJECT_TO_JSVAL(obj); /* root */
  {
    SV *nsv = NULL;
    if (svt == SVt_PVAV) {
      /* arrayref */
      AV *nav;
      nav = newAV();
      nsv = newRV_inc((SV *)nav);
      SvREFCNT_dec((SV *)nav);
    } else if (svt == SVt_PVHV) {
      /* hashref */
      HV *nhv;
      nhv = newHV();
      nsv = newRV_inc((SV *)nhv);
      SvREFCNT_dec((SV *)nhv);
    } else {
      /* SVtPV */
      if (argc > 0) {
	nsv = pmc_jsval_to_sv(aTHX_ ppl, cx, argv[0], 0);
	if (!nsv) {
	  return JS_FALSE;
	}
      } else {
	nsv = newSV(0);
      }
    }
    suc = pmc_plproxy_attach_plpriv(aTHX_ ppl, cx, obj, nsv);
    SvREFCNT_dec(nsv);
    if (!suc) {
      (*rval) = JSVAL_VOID;
    }
  }
  if (suc && argc > 0 && JSVAL_IS_OBJECT(argv[0])) {
    JSObject *arg;
    jsuint arrlen = 0;
    arg = JSVAL_TO_OBJECT(argv[0]);
    if (svt == SVt_PVAV && JS_IsArrayObject(cx, arg) &&
      JS_GetArrayLength(cx, arg, &arrlen)) {
      jsuint i;
      for (i = 0; i < arrlen; ++i) {
	jsval jv = JSVAL_VOID;
	if (JS_GetElement(cx, arg, (jsint)i, &jv)) {
	  JS_SetElement(cx, obj, (jsint)i, &jv);
	}
      }
    } else if (svt == SVt_PVHV) {
      JSIdArray *idarr = NULL;
      idarr = JS_Enumerate(cx, arg);
      if (!idarr) {
	(*rval) = JSVAL_VOID;
	suc = JS_FALSE;
      } else {
	int len = 0, i = 0;
	jsval idv;
	if (!JS_AddNamedRoot(cx, &idv, "pmc_plproxy_newscalar_internal")) {
	  suc = JS_FALSE;
	} else {
	  len = idarr->length;
	  for (i = 0; i < len; ++i) {
	    jsid id;
	    jsval jv = JSVAL_VOID;
	    JSString *ids;
	    pmc_u16ch_t *u16str;
	    size_t u16len = 0;
	    id = idarr->vector[i];
	    if (!JS_IdToValue(cx, id, &idv)) {
	      break;
	    }
	    ids = JS_ValueToString(cx, idv);
	    if (!ids) {
	      break;
	    }
	    u16str = JS_GetStringChars(ids);
	    u16len = JS_GetStringLength(ids);
	    if (JS_GetUCProperty(cx, arg, u16str, u16len, &jv)) {
	      JS_SetUCProperty(cx, obj, u16str, u16len, &jv);
	    }
	  }
	  JS_RemoveRoot(cx, &idv);
	}
	if (!suc || i < len) {
	  (*rval) = JSVAL_VOID;
	  suc = JS_FALSE;
	}
      }
      if (idarr) {
	JS_DestroyIdArray(cx, idarr);
      }
    }
  }
  return suc;
}

static JSBool
pmc_plproxy_newscalar(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  return pmc_plproxy_newscalar_internal(aTHX_ ppl, cx, obj, argc, argv, rval,
    SVt_PV);
}

static JSBool
pmc_plproxy_newarray(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  return pmc_plproxy_newscalar_internal(aTHX_ ppl, cx, obj, argc, argv, rval,
    SVt_PVAV);
}

static JSBool
pmc_plproxy_newhash(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  return pmc_plproxy_newscalar_internal(aTHX_ ppl, cx, obj, argc, argv, rval,
    SVt_PVHV);
}

static JSBool
pmc_plproxy_isperl(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  pmc_plpriv *pp;
  SV *sv;

  if (argc < 1) {
    return pmc_throw_js_logic_error(cx, "MissingArgs", "pmc_plproxy_isperl",
      "missing arg(s)");
  }
  if (!JSVAL_IS_OBJECT(argv[0])) {
    (*rval) = JSVAL_FALSE;
    return JS_TRUE;
  }
  obj = JSVAL_TO_OBJECT(argv[0]); /* overwrite obj */
  pp = pmc_plproxy_get_plpriv(cx, obj);
  if (!pp) {
    (*rval) = JSVAL_FALSE;
    return JS_TRUE;
  }
  sv = pmc_plpriv_get_sv(pp);
  if (!sv) {
    (*rval) = JSVAL_FALSE;
    return JS_TRUE;
  }
  (*rval) = JSVAL_TRUE;
  return JS_TRUE;
}

static JSBool
pmc_funccall(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj, uintN argc,
  jsval *argv, jsval *rval, const char *funcname, int is_method)
{
  return pmc_funccall_scalarctx(aTHX_ ppl, cx, obj, argc, argv, rval, funcname,
    is_method);
}

static JSBool
pmc_plproxy_newtiedscalar(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  uintN iargc, i;
  jsval *iargv, irval = JSVAL_VOID;
  JSBool suc = JS_FALSE;

  /* require JavaScript::Util */
  pmc_load_javascript_util(ppl, cx);

  if (argc < 1) {
    return pmc_throw_js_logic_error(cx, "MissingArgs",
      "pmc_plproxy_newtiedscalar", "missing arg(s)");
  }
  /* load module */
  if (!pmc_plproxy_use_byjv(aTHX_ ppl, cx, obj, argv[0])) {
    return JS_FALSE;
  }
  if (!pmc_plproxy_newscalar_internal(aTHX_ ppl, cx, obj, argc, argv, rval,
    SVt_PV)) {
    return JS_FALSE;
  }
  iargc = argc + 1;
  iargv = (jsval *)pmc_malloc(cx, iargc * sizeof(jsval),
    "pmc_plproxy_newtiedscalar");
  if (!argv) {
    return JS_FALSE;
  }
  iargv[0] = (*rval); /* must keep the value of iargv[0] being rooted
    by (*rval) */
  for (i = 0; i < argc; ++i) {
    iargv[i + 1] = argv[i];
  }
  suc = pmc_funccall(aTHX_ ppl, cx, obj, iargc, iargv, &irval,
    "JavaScript::Util::tie_scalar", 0);
  pmc_free(iargv, "pmc_plproxy_newtiedscalar");
  return suc;
}

static JSBool
pmc_plproxy_tie_internal(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  int is_untie, uintN argc, jsval *argv, jsval *rval)
{
  const char *func = NULL;
  pmc_plpriv *pp;
  SV *sv, *svtarget;
  JSBool suc = JS_TRUE;

  /* require JavaScript::Util */
  pmc_load_javascript_util(ppl, cx);

  if (argc < (is_untie ? 1U : 2U)) {
    return pmc_throw_js_logic_error(cx, "MissingArgs",
      "pmc_plproxy_tie_internal", "missing arg(s)");
  }
  /* load module */
  if (!is_untie && !pmc_plproxy_use_byjv(aTHX_ ppl, cx, obj, argv[1])) {
    return JS_FALSE;
  }
  if (!JSVAL_IS_OBJECT(argv[0])) {
    return pmc_throw_js_logic_error(cx, "TypeMismatch",
      "pmc_plproxy_tie_internal", "not a perl value");
  }
  obj = JSVAL_TO_OBJECT(argv[0]); /* overwrite obj */
  pp = pmc_plproxy_get_plpriv(cx, obj);
  if (!pp) {
    /* pp can be null if 'constructor' is fetched */
    (*rval) = JSVAL_VOID;
    return pmc_throw_js_logic_error(cx, "TypeMismatch",
      "pmc_plproxy_tie_internal", "not a perl value");
  }
  sv = pmc_plpriv_get_sv(pp);
  if (!sv) {
    (*rval) = JSVAL_VOID;
    return pmc_throw_js_logic_error(cx, "TypeMismatch",
      "pmc_plproxy_tie_internal", "not a perl value");
  }
  func = is_untie
    ? "JavaScript::Util::untie_scalar"
    : "JavaScript::Util::tie_scalar";
  if (SvROK(sv)) {
    svtarget = SvRV(sv);
    if (!svtarget) {
      /* thru */
    } else if (SvTYPE(svtarget) == SVt_PVHV) {
      func = is_untie
	? "JavaScript::Util::untie_hash"
	: "JavaScript::Util::tie_hash";
    } else if (SvTYPE(svtarget) == SVt_PVAV) {
      func = is_untie
	? "JavaScript::Util::untie_array"
	: "JavaScript::Util::tie_array";
    }
  }
  DBG_TIECNT(fprintf(stderr, "%s pre: %p %d\n", (is_untie ? "UNTIE" : "TIE"),
    sv, (int)SvREFCNT(sv)));
  suc = pmc_funccall(aTHX_ ppl, cx, obj, argc, argv, rval, func, 0);
  DBG_TIECNT(fprintf(stderr, "%s post: %p %d\n", (is_untie ? "UNTIE" : "TIE"),
    sv, (int)SvREFCNT(sv)));
  return suc;
}

static JSBool
pmc_plproxy_tie(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj, uintN argc,
  jsval *argv, jsval *rval)
{
  return pmc_plproxy_tie_internal(aTHX_ ppl, cx, obj, 0, argc, argv, rval);
}

static JSBool
pmc_plproxy_untie(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj, uintN argc,
  jsval *argv, jsval *rval)
{
  return pmc_plproxy_tie_internal(aTHX_ ppl, cx, obj, 1, argc, argv, rval);
}

static JSBool
pmc_plproxy_coercetofunction_call(JSContext *cx, JSObject *obj, uintN argc,
  jsval *argv, jsval *rval)
{
  JSObject *pobj = NULL;
  if (!JSVAL_IS_OBJECT(argv[-2])) {
    (*rval) = JSVAL_VOID;
    return JS_TRUE;
  }
  obj = JSVAL_TO_OBJECT(argv[-2]); /* overwrite obj */
  pobj = JS_GetParent(cx, obj);
  if (!pobj) {
    return pmc_throw_js_logic_error(cx, "InternalError",
      "pmc_plproxy_coercetofunction_call", "parent is null");
  }
  return JS_CallFunctionValue(cx, pobj, OBJECT_TO_JSVAL(pobj), argc,
    argv, rval);
}

static JSBool
pmc_plproxy_coercetofunction(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  JSFunction *fun;
  JSObject *fobj;
  if (argc < 1) {
    return pmc_throw_js_logic_error(cx, "MissingArgs",
      "pmc_plproxy_coercetofunction", "missing arg(s)");
  }
  if (!JSVAL_IS_OBJECT(argv[0])) {
    return pmc_throw_js_logic_error(cx, "TypeMismatch",
      "pmc_plproxy_coercefunction", "invalid argument");
  }
  fun = JS_NewFunction(cx, pmc_plproxy_coercetofunction_call, 0, 0, 
    JSVAL_TO_OBJECT(argv[0]), NULL);
  if (!fun) {
    return JS_FALSE;
  }
  fobj = JS_GetFunctionObject(fun);
  (*rval) = OBJECT_TO_JSVAL(fobj);
  return JS_TRUE;
}

static JSBool
pmc_plproxy_newtiedarray(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  /* require JavaScript::Util */
  pmc_load_javascript_util(ppl, cx);

  if (argc < 1) {
    return pmc_throw_js_logic_error(cx, "MissingArgs",
      "pmc_plproxy_newtiedarray", "missing arg(s)");
  }
  /* load module */
  if (!pmc_plproxy_use_byjv(aTHX_ ppl, cx, obj, argv[0])) {
    return JS_FALSE;
  }
  return pmc_funccall(aTHX_ ppl, cx, obj, argc, argv, rval,
    "JavaScript::Util::new_tied_array", 0);
}

static JSBool
pmc_plproxy_newtiedhash(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  /* require JavaScript::Util */
  pmc_load_javascript_util(ppl, cx);

  if (argc < 1) {
    return pmc_throw_js_logic_error(cx, "MissingArgs",
      "pmc_plproxy_newtiedhash", "missing arg(s)");
  }
  /* load module */
  if (!pmc_plproxy_use_byjv(aTHX_ ppl, cx, obj, argv[0])) {
    return JS_FALSE;
  }
  return pmc_funccall(aTHX_ ppl, cx, obj, argc, argv, rval,
    "JavaScript::Util::new_tied_hash", 0);
}

static int
pmc_plproxy_is_nonobj_hashref_internal(pTHX_ pmc_plpriv *pp)
{
  svtype st;
  int isobj = 0;

  st = pmc_plproxy_get_svtype(aTHX_ pp, &isobj);
  return (st == SVt_PVHV && !isobj);
}

static int
pmc_plproxy_is_nonobj_hashref(pTHX_ JSContext *cx, JSObject *obj)
{
  pmc_plpriv *pp;

  pp = pmc_plproxy_get_plpriv(cx, obj);
  if (!pp) {
    return 0;
  }
  return pmc_plproxy_is_nonobj_hashref_internal(aTHX_ pp);
}

static JSBool
pmc_plproxy_setsv_internal(pTHX_ PMCPerl *ppl, JSContext *cx, SV *dst,
  SV *src)
{
  JSBool suc = JS_TRUE;

  if (SvMAGICAL(dst) && mg_find(dst, PERL_MAGIC_tiedscalar) != NULL) {
    SV *argv[2];
    int rvalc = 0;
    SV **rvalv = NULL;

    /* require JavaScript::Util */
    pmc_load_javascript_util(ppl, cx);

    DBG_SETSV(fprintf(stderr, "setsv magic\n"));
    argv[0] = dst;
    argv[1] = src;
    suc = pmc_funccall_internal(aTHX_ cx, 2, argv, &rvalc, &rvalv,
      "JavaScript::Util::sv_setsv", 0, G_SCALAR);
    pmc_free_svarr(aTHX_ rvalc, rvalv, "pmc_plproxy_setsv_internal");
  } else {
    DBG_SETSV(fprintf(stderr, "setsv nomagic\n"));
    SvSetSV_nosteal(dst, src);
  }
  return suc;
}

static JSBool
pmc_plproxy_setvalue_internal(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval, int binary_flag)
{
  pmc_plpriv *pp;
  JSBool suc = JS_TRUE;
  SV *sv;
  SV *psv;

  pp = pmc_plproxy_get_plpriv(cx, obj);
  if (!pp) {
    return JS_TRUE;
  }
  psv = pmc_plpriv_get_sv(pp);
  if (!psv) {
    return JS_TRUE;
  }
  if (pmc_plproxy_is_nonobj_hashref_internal(aTHX_ pp)) {
    return pmc_throw_js_logic_error(cx, "TypeMismatch",
      "pmc_plproxy_setvalue", "not a plain scalar.");
  }
  if (argc < 1) {
    return pmc_throw_js_logic_error(cx, "MissingArgs",
      "pmc_plproxy_setvalue_internal", "missing arg(s)");
  }
  (*rval) = JSVAL_VOID;
  sv = pmc_jsval_to_sv(aTHX_ ppl, cx, argv[0], binary_flag);
  if (sv) {
    suc = pmc_plproxy_setsv_internal(aTHX_ ppl, cx, psv, sv);
    SvREFCNT_dec(sv);
    suc = JS_TRUE;
  } else {
    suc = JS_FALSE;
  }
  return suc;
}

static JSBool
pmc_plproxy_setvalue(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  return pmc_plproxy_setvalue_internal(aTHX_ ppl, cx, obj, argc, argv, rval,
    0);
}

static JSBool
pmc_plproxy_setbinaryvalue(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  return pmc_plproxy_setvalue_internal(aTHX_ ppl, cx, obj, argc, argv, rval,
    1);
}

static JSBool
pmc_plproxy_getvalue_internal(pTHX_ PMCPerl *ppl, JSContext *cx,
  JSObject *obj, uintN argc, jsval *argv, jsval *rval, pmc_tojs_mode mode)
{
  pmc_plpriv *pp;
  JSBool suc = JS_TRUE;
  SV *sv;

  pp = pmc_plproxy_get_plpriv(cx, obj);
  if (!pp) {
    (*rval) = JSVAL_VOID;
    return JS_TRUE;
  }
  if (pmc_plproxy_is_nonobj_hashref_internal(aTHX_ pp)) {
    return pmc_throw_js_logic_error(cx, "TypeMismatch",
      "pmc_plproxy_getvalue", "not a plain scalar.");
  }
  sv = pmc_plpriv_get_sv(pp);
  if (sv) {
    suc = pmc_sv_to_jsval(aTHX_ ppl, cx, mode, sv, rval);
  } else {
    (*rval) = JSVAL_VOID;
    suc = JS_TRUE; /* not an error */
  }
  return suc;
}

static JSBool
pmc_plproxy_getvalue(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  return pmc_plproxy_getvalue_internal(aTHX_ ppl, cx, obj, argc, argv, rval,
    pmc_tojs_mode_auto);
}

static JSBool
pmc_plproxy_getbinary(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  return pmc_plproxy_getvalue_internal(aTHX_ ppl, cx, obj, argc, argv, rval,
    pmc_tojs_mode_binary);
}

static JSBool
pmc_plproxy_gettext(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  return pmc_plproxy_getvalue_internal(aTHX_ ppl, cx, obj, argc, argv, rval,
    pmc_tojs_mode_text);
}

static JSBool
pmc_plproxy_tostring_simple(JSContext *cx, jsval *rval)
{
  const char s[] = "[object Perl]";
  JSString *str;
  str = JS_NewStringCopyN(cx, s, sizeof(s) - 1);
  if (!str) {
    (*rval) = JS_FALSE;
    return JS_FALSE;
  }
  (*rval) = STRING_TO_JSVAL(str);
  return JS_TRUE;
}

static JSBool
pmc_plproxy_tostring(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  pmc_plpriv *pp;
  JSBool suc = JS_TRUE;
  SV *sv;
  JSString *str;

  (*rval) = JSVAL_VOID;
  pp = pmc_plproxy_get_plpriv(cx, obj);
  if (!pp) {
    DBG_CONV(fprintf(stderr, "tostring0\n"));
    (*rval) = JSVAL_VOID;
    return JS_TRUE;
  }
  if (pmc_plproxy_is_nonobj_hashref_internal(aTHX_ pp)) {
    return pmc_throw_js_logic_error(cx, "TypeMismatch",
      "pmc_plproxy_tostring", "perl hashref has no toString method.");
  }
  sv = pmc_plpriv_get_sv(pp);
  if (!sv) {
    DBG_CONV(fprintf(stderr, "tostring1\n"));
    return pmc_plproxy_tostring_simple(cx, rval);
  }
  if (SvROK(sv)) {
    /* sv is a reference. try to call toString. */
    jsval arg;
    arg = OBJECT_TO_JSVAL(obj);
    DBG_TOSTRING(fprintf(stderr, "TOSTRING: REF\n"));
    suc = pmc_funccall_scalarctx(aTHX_ ppl, cx, obj, 1, &arg, rval,
      "toString", 1);
    if (!suc) {
      DBG_CONV(fprintf(stderr, "tostring2\n"));
      if (!JS_IsExceptionPending(cx)) {
	/* possibly out of memory */
	return JS_FALSE;
      }
      JS_ClearPendingException(cx);
      return pmc_plproxy_tostring_simple(cx, rval);
    }
    DBG_CONV(fprintf(stderr, "tostring3\n"));
    return JS_TRUE;
  }
  /* sv is not a reference */
  DBG_TOSTRING(fprintf(stderr, "TOSTRING: NOTREF\n"));
  suc = pmc_plproxy_getvalue(aTHX_ ppl, cx, obj, argc, argv, rval);
  if (!suc) {
    DBG_CONV(fprintf(stderr, "tostring6\n"));
    (*rval) = JSVAL_VOID;
    return JS_FALSE;
  }
  if (JSVAL_IS_OBJECT(*rval)) {
    DBG_CONV(fprintf(stderr, "tostring4\n"));
    return pmc_plproxy_tostring_simple(cx, rval);
  }
  str = JS_ValueToString(cx, *rval);
  if (!str) {
    DBG_CONV(fprintf(stderr, "tostring7\n"));
    return JS_FALSE;
  }
  (*rval) = STRING_TO_JSVAL(str);
  DBG_CONV(fprintf(stderr, "tostring5\n"));
  return suc;
}

static JSBool
pmc_plproxy_detach(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  if (pmc_plproxy_is_nonobj_hashref(aTHX_ cx, obj)) {
    (*rval) = JSVAL_VOID;
    return pmc_throw_js_logic_error(cx, "TypeMismatch", "pmc_plproxy_detach",
      "use Perl.Detach(x) instead.");
  }
  pmc_plproxy_detach_plpriv(my_perl, ppl, cx, obj);
  (*rval) = JSVAL_VOID;
  return JS_TRUE;
}

static JSBool
pmc_plproxy_eval(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj, uintN argc,
  jsval *argv, jsval *rval)
{
  char *str = NULL;
  char *fname = NULL;
  size_t u8len = 0;
  JSBool suc = JS_TRUE;

  if (argc < 1) {
    return pmc_throw_js_logic_error(cx, "MissingArgs", "pmc_plproxy_eval",
      "missing arg(s)");
  }
  str = pmc_utf_jsval_getstring(cx, argv[0], &u8len, "pmc_plproxy_eval");
  if (!str) {
    (*rval) = JSVAL_VOID;
    suc = JS_FALSE;
  }
  if (suc && argc >= 2) {
    fname = pmc_utf_jsval_getstring(cx, argv[1], &u8len, "pmc_plproxy_eval");
    if (!fname) {
      (*rval) = JSVAL_VOID;
      suc = JS_FALSE;
    }
  }

  if (suc) {
    suc = pmc_eval_internal(aTHX_ ppl, cx, str, fname, rval);
  }

  pmc_free(fname, "pmc_plproxy_eval");
  pmc_free(str, "pmc_plproxy_eval");
  return suc;
}

static JSBool
pmc_plproxy_suppressconv(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  JSBool suc = JS_TRUE;
  int old_suppress = 0;

  if (argc < 1) {
    return pmc_throw_js_logic_error(cx, "MissingArgs",
      "pmc_plproxy_suppressconv", "missing arg(s)");
  }
  old_suppress = pmc_get_suppress_conv(ppl);
  pmc_set_suppress_conv(ppl, 1);
  suc = JS_CallFunctionValue(cx, obj, argv[0], argc - 1, argv + 1, rval);
  pmc_set_suppress_conv(ppl, old_suppress);
  return suc;
}

static JSBool
pmc_plproxy_calljs_internal(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  int new_flag, uintN argc, jsval *argv, jsval *rval)
{
  pmc_jsglobal *pjg;
  JSObject *glo;
  char *str = NULL;
  size_t u8len = 0;
  JSBool suc = JS_FALSE;
  jsval cnstrval = JSVAL_VOID;
  JSClass *clklass = NULL;
  JSObject *robj = NULL;
  JSObject *fnobj = NULL;
  JSFunction *func = NULL;

  DBG_CJS(fprintf(stderr, "pmc_plproxy_calljs\n"));
  if (argc < 1) {
    return pmc_throw_js_logic_error(cx, "MissingArgs",
      "pmc_plproxy_calljs_internal", "missing arg(s)");
  }
  if (!ppl) {
    DBG_CJS(fprintf(stderr, "calljs: ppl\n"));
    return pmc_throw_js_logic_error(cx, "InternalError",
      "pmc_plproxy_calljs_internal", "internal error: ppl is null");
  }
  pjg = pmc_get_pmc_jsglobal(ppl);
  if (!pjg) {
    DBG_CJS(fprintf(stderr, "calljs: pjg\n"));
    return pmc_throw_js_logic_error(cx, "InternalError",
      "pmc_plproxy_calljs_internal", "internal error: pjg is null");
  }
  glo = pmc_jsglobal_getobject(pjg);
  if (!glo) {
    DBG_CJS(fprintf(stderr, "calljs: glo\n"));
    return pmc_throw_js_logic_error(cx, "InternalError",
      "pmc_plproxy_calljs_internal", "internal error: glo is null");
  }
  if (JSVAL_IS_OBJECT(argv[0])) {
    fnobj = JSVAL_TO_OBJECT(argv[0]);
    func = JS_ValueToFunction(cx, argv[0]); /* can be null */
    cnstrval = argv[0];
    DBG_CJS(fprintf(stderr, "calljs: firstarg is funcobj\n"));
  }
  if (!fnobj) {
    /* first arg is not a function. assume it's a string. */
    str = pmc_utf_jsval_getstring(cx, argv[0], &u8len, "pmc_plproxy_calljs");
    DBG_CJS(fprintf(stderr, "calljs: firstarg is string [%s]\n", str));
    if (!str) {
      (*rval) = JSVAL_VOID;
      return JS_FALSE;
    }
    func = NULL;
  }
  if (argc > 1) {
    DBG_CJS(pmc_dump_jsval(cx, "calljs argv1", argv[1]));
  }
  if (!new_flag) {
    /* normal function call */
    if (str) {
      suc = JS_CallFunctionName(cx, glo, str, argc - 1, argv + 1, rval);
      pmc_free(str, "pmc_plproxy_calljs");
    } else {
      assert(fnobj);
      suc = JS_CallFunctionValue(cx, glo, OBJECT_TO_JSVAL(fnobj), argc - 1,
	argv + 1, rval);
    }
    DBG_CJS(pmc_dump_jsval(cx, "calljs retval", *rval));
    return suc;
  }
  /* call with 'new' */
  if (str) {
    DBG_CJS(fprintf(stderr, "calljs: str=[%s]\n", str));
    suc = pmc_utf_getproperty(cx, glo, str, 1, &cnstrval);
    pmc_free(str, "pmc_plproxy_calljs");
    if (!suc) {
      (*rval) = JSVAL_VOID;
      return JS_FALSE;
    }
    if (JSVAL_IS_OBJECT(cnstrval) &&
      JS_ObjectIsFunction(cx, JSVAL_TO_OBJECT(cnstrval))) {
      func = JS_ValueToFunction(cx, cnstrval);
    }
  }
  if (!func) {
    DBG_CJS(fprintf(stderr, "calljs: not a function\n"));
    return pmc_throw_js_logic_error(cx, "TypeMismatch",
      "pmc_plproxy_calljs_internal", "not a function");
  }
  clklass = func->clasp;
  if (clklass) {
    DBG_CJS(fprintf(stderr, "newobj: class=%s\n", clklass->name));
    robj = JS_ConstructObjectWithArguments(cx, clklass, NULL, NULL,
      argc - 1, argv + 1);
    if (!robj) {
      DBG_CJS(fprintf(stderr, "calljs: construct failed\n"));
      return pmc_throw_js_runtime_exception(cx, "JSNewFailed",
	"pmc_plproxy_calljs_internal", "new failed");
    }
  } else {
    /* not a native class */
    JSObject *arr = NULL;
    robj = JS_NewObject(cx, NULL, NULL, NULL);
    if (!robj) {
      return JS_FALSE;
    }
    (*rval) = OBJECT_TO_JSVAL(robj); /* root */
    if (!JS_AddNamedRoot(cx, &arr, "pmc_plproxy_calljs_internal")) {
      return JS_FALSE;
    }
    arr = JS_NewArrayObject(cx, argc - 1, NULL);
    if (arr) {
      uintN i = 0;
      for (i = 1; i < argc; ++i) {
	if (!JS_SetElement(cx, arr, i - 1, &argv[i])) {
	  break;
	}
      }
      if (i == argc) {
	jsval apargv[2];
	jsval cr = JSVAL_VOID;
	JSObject *fo;
	apargv[0] = OBJECT_TO_JSVAL(robj);
	apargv[1] = OBJECT_TO_JSVAL(arr);
	fo = JSVAL_TO_OBJECT(cnstrval);
	suc = JS_CallFunctionName(cx, fo, "apply", 2, apargv, &cr);
      } else {
	suc = JS_FALSE;
      }
    } else {
      suc = JS_FALSE;
    }
    JS_RemoveRoot(cx, &arr);
  }
  if (suc) {
    (*rval) = OBJECT_TO_JSVAL(robj);
  } else {
    (*rval) = JSVAL_VOID;
  }
  return suc;
}

static JSBool
pmc_plproxy_calljs(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  return pmc_plproxy_calljs_internal(aTHX_ ppl, cx, obj, 0, argc, argv, rval);
}

static JSBool
pmc_plproxy_newjs(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  return pmc_plproxy_calljs_internal(aTHX_ ppl, cx, obj, 1, argc, argv, rval);
}

static JSBool
pmc_plproxy_bintotext(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  if (argc < 1) {
    return pmc_throw_js_logic_error(cx, "MissingArgs",
      "pmc_plproxy_bintotext", "missing arg(s)");
  }
  if (JSVAL_IS_STRING(argv[0])) {
    JSString *str;
    const char *u8str;
    size_t u8len = 0, u16len = 0;
    pmc_u16ch_t *u16str;
    str = JSVAL_TO_STRING(argv[0]);
    u8str = JS_GetStringBytes(str);
    u8len = JS_GetStringLength(str);
    u16str = pmc_new_utf16str_from_utf8str(cx, u8str, u8len, &u16len,
      "pmc_plproxy_bintotext");
    if (!u16str) {
      return JS_FALSE;
    }
    str = JS_NewUCStringCopyN(cx, u16str, u16len);
    pmc_free(u16str, "pmc_plproxy_bintotext");
    if (!str) {
      return JS_FALSE;
    }
    (*rval) = STRING_TO_JSVAL(str);
  } else {
    (*rval) = argv[0];
  }
  return JS_TRUE;
}

static JSBool
pmc_plproxy_texttobin(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  if (argc < 1) {
    return pmc_throw_js_logic_error(cx, "MissingArgs", "pmc_plproxy_texttobin",
      "missing arg(s)");
  }
  if (JSVAL_IS_STRING(argv[0])) {
    JSString *str;
    char *u8str;
    size_t u8len = 0, u16len = 0;
    const pmc_u16ch_t *u16str;
    str = JSVAL_TO_STRING(argv[0]);
    u16str = JS_GetStringChars(str);
    u16len = JS_GetStringLength(str);
    u8str = pmc_new_utf8str_from_utf16str(cx, u16str, u16len, &u8len,
      "pmc_plproxy_texttobin");
    if (!u8str) {
      return JS_FALSE;
    }
    str = JS_NewStringCopyN(cx, u8str, u8len);
    pmc_free(u8str, "pmc_plproxy_texttobin");
    if (!str) {
      return JS_FALSE;
    }
    (*rval) = STRING_TO_JSVAL(str);
  }
  return JS_TRUE;
}

static JSBool
pmc_plproxy_detachjsproxies(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  if (!ppl) {
    (*rval) = JSVAL_VOID;
    return JS_TRUE;
  }
  pmc_detach_js_proxies(ppl, cx);
  (*rval) = JSVAL_VOID;
  return JS_TRUE;
}

static JSBool
pmc_plproxy_detachperlproxies(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  if (!ppl) {
    (*rval) = JSVAL_VOID;
    return JS_TRUE;
  }
  pmc_detach_perl_proxies(ppl, cx);
  (*rval) = JSVAL_VOID;
  return JS_TRUE;
}

static JSBool
pmc_plproxy_detachproxies(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  pmc_plproxy_detachjsproxies(aTHX_ ppl, cx, obj, argc, argv, rval);
  pmc_plproxy_detachperlproxies(aTHX_ ppl, cx, obj, argc, argv, rval);
  return JS_TRUE;
}

static JSBool
pmc_plproxy_detachone(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  JSClass *cl;
  if (argc < 1) {
    (*rval) = JSVAL_VOID;
    return pmc_throw_js_logic_error(cx, "MissingArgs",
      "pmc_plproxy_detachone", "missing arg(s)");
  }
  if (!JSVAL_IS_OBJECT(argv[0])) {
    (*rval) = JSVAL_VOID;
    return pmc_throw_js_logic_error(cx, "TypeMismatch",
      "pmc_plproxy_detachone", "invalid argument");
  }
  obj = JSVAL_TO_OBJECT(argv[0]); /* overwrite obj */
  if (!obj) {
    (*rval) = JSVAL_VOID;
    return pmc_throw_js_logic_error(cx, "TypeMismatch",
      "pmc_plproxy_detachone", "invalid argument");
  }
  cl = JS_GET_CLASS(cx, obj);
  if (cl != &pmc_perl_class) {
    (*rval) = JSVAL_VOID;
    return pmc_throw_js_logic_error(cx, "TypeMismatch",
      "pmc_plproxy_detachone", "invalid argument");
  }
  pmc_plproxy_detach_plpriv(my_perl, ppl, cx, obj);
  (*rval) = JSVAL_VOID;
  return JS_TRUE;
}

static JSBool
pmc_plproxy_apply(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
  jsval *rval)
{
  JSObject *arr = NULL;
  JSBool suc = JS_TRUE;
  jsuint fargc = 0, i = 0;
  jsval *fargv = NULL;

  if (argc < 2) {
    (*rval) = JSVAL_VOID;
    return pmc_throw_js_logic_error(cx, "MissingArgs", "pmc_plproxy_detachone",
      "missing arg(s)");
  }
  if (JSVAL_IS_OBJECT(argv[1]) &&
    JS_IsArrayObject(cx, JSVAL_TO_OBJECT(argv[1]))) {
    arr = JSVAL_TO_OBJECT(argv[1]);
  } else {
    (*rval) = JSVAL_VOID;
    return pmc_throw_js_logic_error(cx, "TypeMismatch",
      "pmc_plproxy_apply", "invalid argument");
  }
  if (!JS_GetArrayLength(cx, arr, &fargc)) {
    (*rval) = JSVAL_VOID;
    return pmc_throw_js_runtime_exception(cx, "InternalError",
      "pmc_plproxy_apply", "getarraylength failed");
  }

  fargv = (jsval *)pmc_malloc(cx, fargc * sizeof(*fargv), "pmc_plproxy_apply");
  if (!fargv) {
    (*rval) = JSVAL_VOID;
    return JS_FALSE;
  }
  for (i = 0; i < fargc; ++i) {
    if (!JS_GetElement(cx, arr, i, fargv + i)) {
      pmc_throw_js_runtime_exception(cx, "InternalError", "pmc_plproxy_apply",
	"getelement failed");
      (*rval) = JSVAL_VOID;
      suc = JS_FALSE;
      goto out;
    }
  }
  suc = JS_CallFunctionValue(cx, obj, argv[0], fargc, fargv, rval);

 out:

  pmc_free(fargv, "pmc_plproxy_apply");
  return suc;
}

typedef enum {
  pmc_plproxy_container_op_get,
  pmc_plproxy_container_op_set,
  pmc_plproxy_container_op_del,
  pmc_plproxy_container_op_fetchsize,
} pmc_plproxy_container_op;

static JSBool
pmc_plproxy_container_op_tied(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  svtype ty, jsval idx, pmc_plproxy_container_op op, jsval *vp)
{
  jsval argv[3];
  uintN argc = 0;
  const char *func = NULL;
  jsval rval = JSVAL_VOID;
  JSBool suc = JS_FALSE;

  /* require JavaScript::Util */
  pmc_load_javascript_util(ppl, cx);

  argv[0] = OBJECT_TO_JSVAL(obj);
  argv[1] = idx;
  argc = 2;
  if (ty == SVt_PVHV) {
    switch (op) {
    case pmc_plproxy_container_op_get:
      func = "JavaScript::Util::hv_fetch";
      break;
    case pmc_plproxy_container_op_set:
      argc = 3;
      argv[2] = (*vp);
      func = "JavaScript::Util::hv_store";
      break;
    case pmc_plproxy_container_op_del:
      func = "JavaScript::Util::hv_delete";
      break;
    case pmc_plproxy_container_op_fetchsize:
      (*vp) = JSVAL_VOID;
      return JS_TRUE;
    }
  } else if (ty == SVt_PVAV) {
    switch (op) {
    case pmc_plproxy_container_op_get:
      func = "JavaScript::Util::av_fetch";
      break;
    case pmc_plproxy_container_op_set:
      argc = 3;
      argv[2] = (*vp);
      func = "JavaScript::Util::av_store";
      break;
    case pmc_plproxy_container_op_del:
      func = "JavaScript::Util::av_delete";
      break;
    case pmc_plproxy_container_op_fetchsize:
      func = "JavaScript::Util::av_fetchsize";
      argc = 1;
      break;
    }
  } else {
    return JS_TRUE;
  }
  DBG_TIED(fprintf(stderr, "container op tied: %s\n", func));
  DBG_TIED(pmc_dump_jsval(cx, "container op arg", idx));
  suc = pmc_funccall_scalarctx(aTHX_ ppl, cx, obj, argc, argv, &rval, func, 0);
  switch (op) {
  case pmc_plproxy_container_op_set:
  case pmc_plproxy_container_op_del:
    /* set/remove returns void */
    (*vp) = JSVAL_VOID;
    break;
  case pmc_plproxy_container_op_get:
    if (suc) {
      /* get success */
      (*vp) = rval;
      DBG_TIED(fprintf(stderr, "container get tied success\n"));
      DBG_TIED(pmc_dump_jsval(cx, "container get ret", rval));
    } else {
      if (!JS_IsExceptionPending(cx)) {
	/* possibly out of memory */
	(*vp) = JSVAL_VOID;
	suc = JS_FALSE;
      } else {
	DBG_TIED(fprintf(stderr, "container get tied failed\n"));
	/* get failed. don't touch (*vp) */
	JS_ClearPendingException(cx);
	suc = JS_TRUE;
      }
    }
    break;
  case pmc_plproxy_container_op_fetchsize:
    (*vp) = rval;
    break;
  }
  return suc;
}

static JSBool
pmc_plproxy_container_op_do(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval idx, pmc_plproxy_container_op op, jsval *vp)
{
  pmc_plpriv *pp;
  SV *sv, *svtarget;
  JSBool suc = JS_TRUE;
  char *u8key = NULL;
  SV *getsv = NULL, *setsv = NULL;

  pp = pmc_plproxy_get_plpriv(cx, obj);
  if (!pp) {
    /* pp can be null if 'constructor' is fetched */
    (*vp) = JSVAL_VOID;
    return JS_TRUE;
  }
  sv = pmc_plpriv_get_sv(pp);
  if (!sv) {
    (*vp) = JSVAL_VOID;
    return JS_TRUE;
  }
  if (!SvROK(sv)) {
    (*vp) = JSVAL_VOID;
    return JS_TRUE;
  }
  svtarget = SvRV(sv);
  if (!svtarget) {
    (*vp) = JSVAL_VOID;
    return JS_TRUE;
  }
  if ((op == pmc_plproxy_container_op_set ||
       op == pmc_plproxy_container_op_del)
    && SvREADONLY(svtarget)) {
    (*vp) = JSVAL_VOID;
    return JS_TRUE;
  }

  /* check if svtarget is tied */
  if (SvMAGICAL(svtarget) && mg_find(svtarget, PERL_MAGIC_tied) != NULL) {
    suc = pmc_plproxy_container_op_tied(aTHX_ ppl, cx, obj, SvTYPE(svtarget),
      idx, op, vp);
    return suc;
  }

  if (op == pmc_plproxy_container_op_set) {
    setsv = pmc_jsval_to_sv(aTHX_ ppl, cx, *vp, 0);
    if (!setsv) {
      goto out;
    }
  }

  if (SvTYPE(svtarget) == SVt_PVAV) {
    AV *avtarget;
    int32 iidx = 0;
    avtarget = (AV *)svtarget;
    if (op == pmc_plproxy_container_op_fetchsize) {
      /* nothing to do */
    } else if (JSVAL_IS_INT(idx)) {
      iidx = JSVAL_TO_INT(idx);
    } else if (JSVAL_IS_STRING(idx)) {
      /* don't touch (*vp) */
      suc = JS_TRUE;
      goto out;
    } else {
      jsdouble didx = 0;
      if (!JS_ValueToNumber(cx, idx, &didx)) {
	pmc_throw_js_runtime_exception(cx, "ContainerOpFailed",
	  "pmc_plproxy_container_op", "index out of range");
	(*vp) = JSVAL_VOID;
	suc = JS_FALSE;
	goto out;
      }
      didx = floor(didx);
      if (didx < 0.0 || didx >= (1U<<31)) {
	pmc_throw_js_runtime_exception(cx, "ContainerOpFailed",
	  "pmc_plproxy_container_op", "index out of range");
	(*vp) = JSVAL_VOID;
	suc = JS_FALSE;
	goto out;
      }
      iidx = (int32)didx;
    }
    if (op == pmc_plproxy_container_op_del) {
      av_delete(avtarget, iidx, G_DISCARD);
      suc = JS_TRUE;
    } else if (op == pmc_plproxy_container_op_set) {
      SV **elemp;
      elemp = av_fetch(avtarget, iidx, 1);
      if (elemp) {
	SvSetSV((*elemp), setsv);
	SvSETMAGIC(*elemp);
	suc = JS_TRUE;
      } else {
	pmc_throw_js_runtime_exception(cx, "ContainerOpFailed",
	  "pmc_plproxy_container_op", "store failed");
	(*vp) = JSVAL_VOID;
	suc = JS_FALSE;
      }
    } else if (op == pmc_plproxy_container_op_fetchsize) {
      I32 len;
      len = av_len(avtarget) + 1;
      suc = pmc_sv_to_int(cx, len, vp);
      DBG_FETCHSIZE(fprintf(stderr, "op_fetchsize\n"));
    } else {
      SV **elemp;
      elemp = av_fetch(avtarget, iidx, 0);
      if (elemp) {
	getsv = (*elemp);
	SvREFCNT_inc(getsv);
	SvGETMAGIC(getsv);
	suc = JS_TRUE;
      } else {
	/* don't touch (*vp) */
	suc = JS_TRUE;
      }
    }
    DBG_FETCH(fprintf(stderr, "fetch arrelem idx=%d %p\n", iidx, getsv));
  } else if (SvTYPE(svtarget) == SVt_PVHV) {
    HV *hvtarget;
    JSString *str;
    pmc_u16ch_t *u16key;
    size_t u16klen, u8klen;
    hvtarget = (HV *)svtarget;
    str = JS_ValueToString(cx, idx);
    if (!str) {
      suc = JS_FALSE;
      goto out;
    }
    u16key = JS_GetStringChars(str);
    u16klen = JS_GetStringLength(str);
    DBG_FETCH(fprintf(stderr, "u16klen=%d\n", (int)u16klen));
    u8key = pmc_new_utf8str_from_utf16str(cx, u16key, u16klen, &u8klen,
      "pmc_plproxy_container_op_do");
    if (!u8key) {
      (*vp) = JSVAL_VOID;
      goto out;
    }
    DBG_FETCH(fprintf(stderr, "u8klen=%d\n", (int)u8klen));

    if (op == pmc_plproxy_container_op_del) {
      hv_delete(hvtarget, u8key, -(I32)u8klen, G_DISCARD);
      suc = JS_TRUE;
    } else if (op == pmc_plproxy_container_op_set) {
      SV **elemp;
      elemp = hv_fetch(hvtarget, u8key, -(I32)u8klen, 1);
      if (elemp) {
	SvSetSV((*elemp), setsv);
	SvSETMAGIC(*elemp);
	suc = JS_TRUE;
      } else {
	pmc_throw_js_runtime_exception(cx, "ContainerOpFailed",
	  "pmc_plproxy_container_op", "store failed");
	(*vp) = JSVAL_VOID;
	suc = JS_FALSE;
      }
    } else {
      SV **elemp;
      elemp = hv_fetch(hvtarget, u8key, -(I32)u8klen, 0);
      if (elemp) {
	getsv = (*elemp);
	SvREFCNT_inc(getsv);
	SvGETMAGIC(getsv);
	suc = JS_TRUE;
      } else {
	/* don't touch (*vp) */
	suc = JS_TRUE;
      }
    }
    DBG_FETCH(fprintf(stderr, "fetch hashelem %p\n", getsv));
    DBG_FETCH(pmc_dump_str("key=", u8key, u8klen));
  }

  if (getsv) {
    if (!pmc_sv_to_jsval(aTHX_ ppl, cx,
      pmc_plproxy_get_tojs_current_mode(ppl), getsv, vp)) {
      suc = JS_FALSE;
    }
  }

 out:

  if (setsv) {
    SvREFCNT_dec(setsv);
  }
  if (getsv) {
    SvREFCNT_dec(getsv);
  }
  if (u8key) {
    pmc_free(u8key, "pmc_plproxy_container_op_do");
  }
  if (!suc) {
    (*vp) = JSVAL_VOID;
  }
  return suc;
}

static JSBool
pmc_plproxy_getcontainerelem(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval idx, jsval *vp)
{
  DBG_ARR(pmc_dump_jsval(cx, "pmc_plproxy_getcontainerelem", idx));
  return pmc_plproxy_container_op_do(aTHX_ ppl, cx, obj, idx,
    pmc_plproxy_container_op_get, vp);
}

static JSBool
pmc_plproxy_setcontainerelem(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval idx, jsval *vp)
{
  pmc_plpriv *pp = NULL;
  pp = pmc_plproxy_get_plpriv(cx, obj);
  DBG_ARR(pmc_dump_jsval(cx, "pmc_plproxy_setcontainerelem", idx));
  return pmc_plproxy_container_op_do(aTHX_ ppl, cx, obj, idx,
    pmc_plproxy_container_op_set, vp);
}

static JSBool
pmc_plproxy_delcontainerelem(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval idx, jsval *vp)
{
  pmc_plpriv *pp = NULL;
  pp = pmc_plproxy_get_plpriv(cx, obj);
  DBG_ARR(pmc_dump_jsval(cx, "pmc_plproxy_delcontainerelem", idx));
  return pmc_plproxy_container_op_do(aTHX_ ppl, cx, obj, idx,
    pmc_plproxy_container_op_del, vp);
}

static JSBool
pmc_plproxy_container_op_do_str(pTHX_ PMCPerl *ppl, JSContext *cx,
  JSObject *obj, const char *idx, pmc_plproxy_container_op op, jsval *vp)
{
  JSString *str;
  jsval jv = JSVAL_VOID;
  JSBool suc = JS_FALSE;
  JS_AddNamedRoot(cx, &jv, "pmc_plproxy_container_op_do_str");
  str = JS_NewStringCopyZ(cx, idx);
  if (str) {
    jv = STRING_TO_JSVAL(str);
    suc = pmc_plproxy_container_op_do(aTHX_ ppl, cx, obj, jv, op, vp);
  } else {
    suc = JS_FALSE;
  }
  JS_RemoveRoot(cx, &jv);
  return suc;
}

static JSBool
pmc_plproxy_getlength(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval id, jsval *vp)
{
  pmc_plpriv *pp;
  JSBool suc = JS_TRUE;
  SV *sv;

  DBG_LEN(fprintf(stderr, "len\n"));
  pp = pmc_plproxy_get_plpriv(cx, obj);
  if (!pp) {
    (*vp) = JSVAL_VOID;
    suc = JS_TRUE; /* not an error */
  }
  sv = pmc_plpriv_get_sv(pp);
  if (sv) {
    if (SvROK(sv)) {
      /* reference */
      SV *svtarget;
      DBG_LEN(fprintf(stderr, "len : ref\n"));
      svtarget = SvRV(sv);
      if (svtarget && SvTYPE(svtarget) == SVt_PVAV) {
	/* array */
	suc = pmc_plproxy_container_op_do(aTHX_ ppl, cx, obj, JSVAL_VOID,
	  pmc_plproxy_container_op_fetchsize, vp);
      }
    } else if (SvPOK(sv)) {
      /* string */
      STRLEN len = SvLEN(sv);
      (*vp) = INT_TO_JSVAL(len);
    }
  } else {
    /* behave as normal property */
    return pmc_plproxy_container_op_do_str(aTHX_ ppl, cx, obj, "length",
      pmc_plproxy_container_op_get, vp);
  }
  return suc;
}

static JSBool
pmc_plproxy_setlength(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval id, jsval *vp)
{
  pmc_plpriv *pp;
  JSBool suc = JS_TRUE;
  svtype st;
  int isobj = 0;

  pp = pmc_plproxy_get_plpriv(cx, obj);
  if (!pp) {
    (*vp) = JSVAL_VOID;
    suc = JS_TRUE; /* not an error */
  }
  st = pmc_plproxy_get_svtype(aTHX_ pp, &isobj);
  if (st == SVt_PVHV && !isobj) {
    return pmc_plproxy_container_op_do_str(aTHX_ ppl, cx, obj, "length",
      pmc_plproxy_container_op_set, vp);
  }
  (*vp) = JSVAL_VOID;
  return JSVAL_FALSE;
}

static JSBool
pmc_plproxy_getproto(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval id, jsval *vp)
{
  return pmc_plproxy_container_op_do_str(aTHX_ ppl, cx, obj, "__proto__",
    pmc_plproxy_container_op_get, vp);
}

static JSBool
pmc_plproxy_setproto(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval id, jsval *vp)
{
  return pmc_plproxy_container_op_do_str(aTHX_ ppl, cx, obj, "__proto__",
    pmc_plproxy_container_op_set, vp);
}

static JSBool
pmc_plproxy_getparent(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval id, jsval *vp)
{
  return pmc_plproxy_container_op_do_str(aTHX_ ppl, cx, obj, "__parent__",
    pmc_plproxy_container_op_get, vp);
}

static JSBool
pmc_plproxy_setparent(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval id, jsval *vp)
{
  return pmc_plproxy_container_op_do_str(aTHX_ ppl, cx, obj, "__parent__",
    pmc_plproxy_container_op_set, vp);
}

static JSBool
pmc_plproxy_getcount(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval id, jsval *vp)
{
  return pmc_plproxy_container_op_do_str(aTHX_ ppl, cx, obj, "__count__",
    pmc_plproxy_container_op_get, vp);
}

static JSBool
pmc_plproxy_setcount(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval id, jsval *vp)
{
  return pmc_plproxy_container_op_do_str(aTHX_ ppl, cx, obj, "__count__",
    pmc_plproxy_container_op_set, vp);
}

static JSBool
pmc_plproxy_getfunctions_internal(pTHX_ PMCPerl *ppl, JSContext *cx,
  JSObject *obj, pmc_plpriv_calltype ct, jsval id, jsval *vp)
{
  DBG_LEN(fprintf(stderr, "len\n"));
  return pmc_plproxy_new_pkgobject(aTHX_ ppl, cx, ct, NULL, NULL, vp);
}

static JSBool
pmc_plproxy_getfunctions(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval id, jsval *vp)
{
  return pmc_plproxy_getfunctions_internal(aTHX_ ppl, cx, obj,
    pmc_plpriv_calltype_pkgfunc, id, vp);
}

static JSBool
pmc_plproxy_getmethods(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval id, jsval *vp)
{
  return pmc_plproxy_getfunctions_internal(aTHX_ ppl, cx, obj,
    pmc_plpriv_calltype_pkgmethod, id, vp);
}

static JSBool
pmc_plproxy_getvariables(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval id, jsval *vp)
{
  return pmc_plproxy_getfunctions_internal(aTHX_ ppl, cx, obj,
    pmc_plpriv_calltype_pkgvariable, id, vp);
}

static JSBool
pmc_plproxy_getarrays(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval id, jsval *vp)
{
  return pmc_plproxy_getfunctions_internal(aTHX_ ppl, cx, obj,
    pmc_plpriv_calltype_pkgarray, id, vp);
}

static JSBool
pmc_plproxy_gethashes(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval id, jsval *vp)
{
  return pmc_plproxy_getfunctions_internal(aTHX_ ppl, cx, obj,
    pmc_plpriv_calltype_pkghash, id, vp);
}

static JSBool
pmc_plproxy_gethandles(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval id, jsval *vp)
{
  return pmc_plproxy_getfunctions_internal(aTHX_ ppl, cx, obj,
    pmc_plpriv_calltype_pkghandle, id, vp);
}

static JSBool
pmc_plproxy_getconstants(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval id, jsval *vp)
{
  return pmc_plproxy_getfunctions_internal(aTHX_ ppl, cx, obj,
    pmc_plpriv_calltype_pkgconst, id, vp);
}

static JSBool
pmc_plproxy_getfunctionslc(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval id, jsval *vp)
{
  return pmc_plproxy_getfunctions_internal(aTHX_ ppl, cx, obj,
    pmc_plpriv_calltype_pkgfunc_listcontext, id, vp);
}

static JSBool
pmc_plproxy_getmethodslc(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval id, jsval *vp)
{
  return pmc_plproxy_getfunctions_internal(aTHX_ ppl, cx, obj,
    pmc_plpriv_calltype_pkgmethod_listcontext, id, vp);
}

static JSBool
pmc_plproxy_getconstantslc(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval id, jsval *vp)
{
  return pmc_plproxy_getfunctions_internal(aTHX_ ppl, cx, obj,
    pmc_plpriv_calltype_pkgconst_listcontext, id, vp);
}

static JSBool
pmc_plproxy_getrawfunctions(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval id, jsval *vp)
{
  return pmc_plproxy_getfunctions_internal(aTHX_ ppl, cx, obj,
    pmc_plpriv_calltype_pkgrawfunc, id, vp);
}

static JSBool
pmc_plproxy_setdummy(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval id, jsval *vp)
{
  (*vp) = JSVAL_VOID;
  return JS_TRUE;
}

static JSBool
pmc_plproxy_geterrno(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval id, jsval *vp)
{
  return pmc_plproxy_getvar_internal(aTHX_ ppl, cx, "!", vp);
}

static JSBool
pmc_plproxy_seterrno(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval id, jsval *vp)
{
  return pmc_plproxy_setvar_internal(aTHX_ ppl, cx, "!", vp);
}

static JSBool
pmc_plproxy_getarg(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj, jsval id,
  jsval *vp)
{
  return pmc_plproxy_getvar_internal(aTHX_ ppl, cx, "_", vp);
}

static JSBool
pmc_plproxy_setarg(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj, jsval id,
  jsval *vp)
{
  return pmc_plproxy_setvar_internal(aTHX_ ppl, cx, "_", vp);
}

static JSBool
pmc_plproxy_geta(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj, jsval id,
  jsval *vp)
{
  return pmc_plproxy_getvar_internal(aTHX_ ppl, cx, "a", vp);
}

static JSBool
pmc_plproxy_seta(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj, jsval id,
  jsval *vp)
{
  return pmc_plproxy_setvar_internal(aTHX_ ppl, cx, "a", vp);
}

static JSBool
pmc_plproxy_getb(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj, jsval id,
  jsval *vp)
{
  return pmc_plproxy_getvar_internal(aTHX_ ppl, cx, "b", vp);
}

static JSBool
pmc_plproxy_setb(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj, jsval id,
  jsval *vp)
{
  return pmc_plproxy_setvar_internal(aTHX_ ppl, cx, "b", vp);
}

static JSBool
pmc_plproxy_getchilderr(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval id, jsval *vp)
{
  /* require JavaScript::Util */
  pmc_load_javascript_util(ppl, cx);

  return pmc_funccall(aTHX_ ppl, cx, obj, 0, NULL, vp,
    "JavaScript::Util::child_error", 0);
}

static JSBool
pmc_plproxy_setchilderr(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval id, jsval *vp)
{
  jsval argv[1];

  /* require JavaScript::Util */
  pmc_load_javascript_util(ppl, cx);

  argv[0] = (*vp);
  return pmc_funccall(aTHX_ ppl, cx, obj, 1, argv, vp,
    "JavaScript::Util::child_error", 0);
}

static char *
pmc_plproxy_get_fullname(JSContext *cx, const char *pkgname, const char *name,
  const char *debugid)
{
  char *fullname;
  size_t pnlen = 0, nlen = 0, fulllen = 0;
  pnlen = strlen(pkgname);
  nlen = strlen(name);
  fulllen = pnlen + 2 + nlen;
  fullname = (char *)pmc_malloc(cx, fulllen + 1, debugid);
  if (!fullname) {
    return NULL;
  }
  memcpy(fullname, pkgname, pnlen);
  memcpy(fullname + pnlen, "::", 2);
  memcpy(fullname + pnlen + 2, name, nlen);
  fullname[fulllen] = '\0';
  return fullname;
}

static JSBool
pmc_plproxy_callpkg(pTHX_ PMCPerl *ppl, JSContext *cx, pmc_plpriv_calltype ct,
  const char *pkgname, const char *funcname, uintN argc, jsval *argv,
  jsval *rval)
{
  size_t i = 0;
  JSBool suc = JS_TRUE;
  int is_method = 0;
  SV **svargv = NULL;
  size_t svargc = 0;
  SV **rvalv = NULL;
  int rvalc = 0;
  int svargc_inc = 0;
  I32 evalctx = G_SCALAR;

  switch (ct) {
  case pmc_plpriv_calltype_pkgfunc_listcontext:
  case pmc_plpriv_calltype_pkgconst_listcontext:
  case pmc_plpriv_calltype_pkgmethod_listcontext:
    evalctx = G_ARRAY;
    break;
  default:
    break;
  }
  svargc_inc =
    (ct == pmc_plpriv_calltype_pkgmethod ||
     ct == pmc_plpriv_calltype_pkgmethod_listcontext)
    ? 1 : 0;
  is_method = svargc_inc;
  svargc = svargc_inc + argc;
  if (svargc != 0) {
    svargv = (SV **)pmc_malloc(cx, svargc * sizeof(SV *),
      "pmc_plproxy_callpkg");
    if (!svargv) {
      (*rval) = JSVAL_VOID;
      goto out;
    }
    for (i = 0; i < argc; ++i) {
      svargv[i + svargc_inc] = pmc_jsval_to_sv(aTHX_ ppl, cx, argv[i], 0);
      if (svargv[i + svargc_inc] == NULL) {
	suc = JS_FALSE;
      }
    }
  }
  if (svargc_inc != 0) {
    svargv[0] = newSVpv(pkgname, 0);
    SvUTF8_on(svargv[0]);
  }
  if (!suc) {
    goto out;
  }
  if (is_method) {
    suc = pmc_funccall_internal(aTHX_ cx, svargc, svargv, &rvalc, &rvalv,
      funcname, is_method, evalctx);
  } else {
    char *full_funcname;
    full_funcname = pmc_plproxy_get_fullname(cx, pkgname, funcname,
      "pmc_plproxy_callpkg");
    if (full_funcname) {
      suc = pmc_funccall_internal(aTHX_ cx, svargc, svargv, &rvalc, &rvalv,
	full_funcname, is_method, evalctx);
      pmc_free(full_funcname, "pmc_plproxy_callpkg");
    } else {
      (*rval) = JSVAL_VOID;
      suc = JS_FALSE;
    }
  }
  if (suc) {
    if (evalctx == G_SCALAR) {
      if (rvalc > 0) {
        suc = pmc_sv_to_jsval(aTHX_ ppl, cx,
	  pmc_plproxy_get_tojs_current_mode(ppl), rvalv[0], rval);
      } else {
        (*rval) = JSVAL_VOID;
        suc = JS_TRUE;
      }
    } else {
      suc = pmc_sv_to_jsval_array(aTHX_ ppl, cx,
	pmc_plproxy_get_tojs_current_mode(ppl), rvalc, rvalv, rval);
    }
  } else {
    (*rval) = JSVAL_VOID;
  }

 out:

  if (rvalv) {
    pmc_free_svarr(aTHX_ rvalc, rvalv, "pmc_plproxy_callpkg");
  }
  if (svargv) {
    pmc_free_svarr(aTHX_ svargc, svargv, "pmc_plproxy_callpkg");
  }
  return suc;
}

static JSBool
pmc_plproxy_getsetvar(pTHX_ PMCPerl *ppl, JSContext *cx,
  pmc_plpriv_calltype ct, const char *pkgname, const char *varname,
  int is_set, jsval *vp)
{
  char *full_varname;
  JSBool suc = JS_TRUE;
  full_varname = pmc_plproxy_get_fullname(cx, pkgname, varname,
    "pmc_plproxy_getsetvar");
  if (!full_varname) {
    (*vp) = JSVAL_VOID;
    return JS_FALSE;
  }
  DBG_VAR(fprintf(stderr, "pmc_plproxy_getsetvar: %s\n", full_varname));
  if (is_set) {
    SV *sv = NULL;
    SV *valsv = NULL;
    valsv = pmc_jsval_to_sv(aTHX_ ppl, cx, (*vp), 0);
    if (!valsv) {
      (*vp) = JSVAL_VOID;
      return JS_FALSE;
    } else {
      sv = get_sv(full_varname, 1);
      if (!sv) {
	(*vp) = JSVAL_VOID;
      } else {
	if (sv != valsv) {
	  sv_setsv(sv, valsv);
	}
      }
      SvREFCNT_dec(valsv);
    }
    DBG_ENABLE_LEAK_DUMMY(pmc_malloc(10, "hey")); /* leak test */
  } else {
    SV *sv = NULL;
    HV *hv = NULL;
    AV *av = NULL;
    GV *gv = NULL;
    switch (ct) {
    case pmc_plpriv_calltype_pkgvariable:
      sv = get_sv(full_varname, 0);
      if (sv) {
	SvREFCNT_inc(sv);
      }
      break;
    case pmc_plpriv_calltype_pkgarray:
      av = get_av(full_varname, 0);
      if (av) {
	SV *svav = (SV *)av;
	sv = newRV_inc(svav);
      }
      break;
    case pmc_plpriv_calltype_pkghash:
      hv = get_hv(full_varname, 0);
      if (hv) {
	SV *svhv = (SV *)hv;
	sv = newRV_inc(svhv);
      }
      break;
    case pmc_plpriv_calltype_pkghandle:
      gv = gv_fetchpv(full_varname, 0, SVt_PVIO);
      if (gv) {
	struct io *iv = NULL;
	iv = GvIO(gv);
	if (iv) {
	  SV *sviv = (SV *)iv;
	  sv = newRV_inc(sviv);
	}
      }
      break;
    default:
      break;
    }
    if (!sv) {
      (*vp) = JSVAL_VOID;
    } else {
      suc = pmc_sv_to_jsval(aTHX_ ppl, cx,
	pmc_plproxy_get_tojs_current_mode(ppl), sv, vp);
      SvREFCNT_dec(sv);
    }
  }
  pmc_free(full_varname, "pmc_plproxy_getsetvar");
  return suc;
}

static JSBool
pmc_plproxy_getsetpkg(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  pmc_plpriv *pp, jsval id, int is_set, jsval *vp)
{
  char *u8str;
  size_t u8len = 0;
  JSBool suc = JS_TRUE;
  pmc_plpriv_calltype ct = pmc_plpriv_calltype_none;
  const char *pkgname = NULL;
  const char *funcname = NULL;

  assert(pp);
  u8str = pmc_utf_jsval_getstring(cx, id, &u8len, "pmc_plproxy_getsetpkg");
  if (!u8str) {
    (*vp) = JSVAL_VOID;
    return JS_FALSE;
  }
  ct = pmc_plpriv_get_calltype(pp);
  pkgname = pmc_plpriv_get_package(pp);
  funcname = pmc_plpriv_get_function(pp);
  if (pkgname == NULL) {
    if (is_set) {
      suc = pmc_throw_js_logic_error(cx, "NotWritable",
	"pmc_plproxy_getsetpkg", "failed to set property");
    } else {
      if (ct == pmc_plpriv_calltype_pkgrawfunc) {
	/* create optimized 'functions' object */
	suc = pmc_plproxy_new_pkgobject_stash(aTHX_ ppl, cx, ct, u8str, vp);
      } else if (ct == pmc_plpriv_calltype_pkgfunc &&
	u8len == 4 && strcmp(u8str, "CORE") == 0) {
	/* require JavaScript::CORE */
	pmc_load_javascript_core(ppl, cx);
	suc = pmc_plproxy_new_pkgobject_stash(aTHX_ ppl, cx, ct,
	  "JavaScript::CORE", vp);
      } else {
	/* set pkgname */
	suc = pmc_plproxy_use_bystr(aTHX_ ppl, cx, obj, u8str)
	  && pmc_plproxy_new_pkgobject(aTHX_ ppl, cx, ct, u8str, NULL, vp);
      }
    }
  } else if (funcname != NULL) {
    suc = pmc_throw_js_logic_error(cx, "PropertyNotFound",
      "pmc_plproxy_getsetpkg", "failed to get property");
  } else {
    if (is_set) {
      if (ct != pmc_plpriv_calltype_pkgvariable) {
	suc = pmc_throw_js_logic_error(cx, "NotWritable",
	  "pmc_plproxy_getsetpkg", "failed to set property");
      } else {
	suc = pmc_plproxy_getsetvar(aTHX_ ppl, cx, ct, pkgname, u8str, 1, vp);
      }
    } else {
      switch (ct) {
      case pmc_plpriv_calltype_pkgfunc:
      case pmc_plpriv_calltype_pkgfunc_listcontext:
      case pmc_plpriv_calltype_pkgmethod:
      case pmc_plpriv_calltype_pkgmethod_listcontext:
	suc = pmc_plproxy_new_pkgobject(aTHX_ ppl, cx, ct, pkgname, u8str, vp);
	break;
      case pmc_plpriv_calltype_pkgconst:
      case pmc_plpriv_calltype_pkgconst_listcontext:
	suc = pmc_plproxy_callpkg(aTHX_ ppl, cx, ct, pkgname, u8str, 0, NULL,
	  vp);
	break;
      case pmc_plpriv_calltype_pkgvariable:
      case pmc_plpriv_calltype_pkgarray:
      case pmc_plpriv_calltype_pkghash:
      case pmc_plpriv_calltype_pkghandle:
	suc = pmc_plproxy_getsetvar(aTHX_ ppl, cx, ct, pkgname, u8str, 0, vp);
	break;
      default:
	suc = pmc_throw_js_logic_error(cx, "InternalError",
	  "pmc_plproxy_getsetpkg", "internal error");
      }
    }
  }
  pmc_free(u8str, "pmc_plproxy_getsetpkg");
  return suc;
}

static JSBool
pmc_plproxy_plgetprop(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  if (pmc_plproxy_is_nonobj_hashref(aTHX_ cx, obj)) {
    (*rval) = JSVAL_VOID;
    return pmc_throw_js_logic_error(cx, "TypeMismatch", "pmc_plproxy_detach",
      "use x['...'] instead.");
  }
  if (argc < 1) {
    return pmc_throw_js_logic_error(cx, "MissingArgs",
      "pmc_plproxy_plgetprop", "missing arg(s)");
  }
  return pmc_plproxy_getcontainerelem(aTHX_ ppl, cx, obj, argv[0], rval);
}

static JSBool
pmc_plproxy_plsetprop(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  JSBool suc = JS_TRUE;
  if (pmc_plproxy_is_nonobj_hashref(aTHX_ cx, obj)) {
    (*rval) = JSVAL_VOID;
    return pmc_throw_js_logic_error(cx, "TypeMismatch", "pmc_plproxy_detach",
      "use x['...'] = y instead.");
  }
  if (argc < 2) {
    return pmc_throw_js_logic_error(cx, "MissingArgs",
      "pmc_plproxy_plsetprop", "missing arg(s)");
  }
  suc = pmc_plproxy_setcontainerelem(aTHX_ ppl, cx, obj, argv[0], argv + 1);
  (*rval) = argv[1];
  return suc;
}

static JSBool
pmc_plproxy_pldelprop(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  uintN argc, jsval *argv, jsval *rval)
{
  if (pmc_plproxy_is_nonobj_hashref(aTHX_ cx, obj)) {
    (*rval) = JSVAL_VOID;
    return pmc_throw_js_logic_error(cx, "TypeMismatch", "pmc_plproxy_detach",
      "use delete x['...'] instead.");
  }
  if (argc < 1) {
    return pmc_throw_js_logic_error(cx, "MissingArgs",
      "pmc_plproxy_pldelprop", "missing arg(s)");
  }
  return pmc_plproxy_delcontainerelem(aTHX_ ppl, cx, obj, argv[0], rval);
}

static JSBool
pmc_plproxy_getprop(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval id, jsval *vp)
{
  pmc_plpriv *pp;
  pmc_plpriv_calltype ct;
  int has_methods = 1;
  svtype st = SVt_NULL;
  int isobj = 0;
  JSString *jstr = NULL;
  JSBool suc = JS_TRUE;

  DBG_GETPROP(pmc_dump_jsval(cx, "pmc_plproxy_getprop", id));
  #if 0
  // FIXME: test caching behavior
  if ((*vp) != JSVAL_VOID) {
    /* (*vp) is possibly a cached method funcobj. */
    return JS_TRUE;
  }
  #endif
  pp = pmc_plproxy_get_plpriv(cx, obj);
  if (!pp) {
    return JS_TRUE;
  }
  ct = pmc_plpriv_get_calltype(pp);
  if (ct != pmc_plpriv_calltype_none) {
    /* obj is a package object */
    return pmc_plproxy_getsetpkg(aTHX_ ppl, cx, obj, pp, id, 0, vp);
  }
  if (JSVAL_IS_INT(id)) {
    jsint tid = 0;
    tid = JSVAL_TO_INT(id);
    if (tid < 0) {
      /* predef property */
      return JS_TRUE;
    }
  }
  /* if sv is a non-blessed hashref, methods are disabled. */
  st = pmc_plproxy_get_svtype(aTHX_ pp, &isobj);
  has_methods = (st != SVt_PVHV || isobj);
  if (has_methods && JSVAL_IS_STRING(id)) {
    pmc_u16ch_t *u16str;
    size_t slen;
    DBG_NOMETHOD(fprintf(stderr, "has_methods\n"));
    jstr = JSVAL_TO_STRING(id);
    u16str = JS_GetStringChars(jstr);
    slen = JS_GetStringLength(jstr);
    if (pmc_utf16_is_7bit_string(u16str, slen)) {
      const char *str;
      const char *methodname = NULL;
      str = JS_GetStringBytes(jstr);
      if (isobj) {
	/* optimization for objects. not so effective though. */
	const char **methodp = NULL;
	for (methodp = pmc_plproxy_methods_for_object;
	  (methodname = (*methodp)) != NULL; ++methodp) {
	  if (strcmp(methodname, str) == 0) {
	    break;
	  }
	}
      } else {
	JSFunctionSpec *methodp = NULL;
	for (methodp = pmc_plproxy_methods;
	  (methodname = methodp->name) != NULL; ++methodp) {
	  DBG_GETPROP(pmc_dump_jsval(cx, "pmc_plproxy_getprop lp", id));
	  if (strcmp(methodname, str) == 0) {
	    break;
	  }
	}
      }
      if (methodname) {
	/* predef function */
	return JS_TRUE;
      }
    }
  } else {
    DBG_NOMETHOD(fprintf(stderr, "no methods\n"));
  }
  DBG_GETPROP(pmc_dump_jsval(cx, "pmc_plproxy_getprop container_op_do", id));
  suc = pmc_plproxy_container_op_do(aTHX_ ppl, cx, obj, id,
    pmc_plproxy_container_op_get, vp);
  /* fetch method */
  if (suc && (*vp) == JSVAL_VOID && isobj) {
    char *name = NULL;
    size_t namelen = 0;
    SV *sv = NULL;
    SV *sv_cref_or_name = NULL;
    DBG_GETPROP(pmc_dump_jsval(cx, "pmc_plproxy_getprop fetchmethod", id));
    name = pmc_utf_jsval_getstring(cx, id, &namelen, "pmc_plproxy_getprop");
    if (!name) {
      (*vp) = JSVAL_VOID;
      return JS_FALSE;
    }
    sv = pmc_plpriv_get_sv(pp);
    if (sv && SvROK(sv)) {
      SV *svtarget;
      HV *stash = NULL;
      GV *gv = NULL;
      CV *cv = NULL;
      svtarget = SvRV(sv);
      if (svtarget) {
	stash = SvSTASH(svtarget);
      }
      if (stash) {
	gv = gv_fetchmethod_autoload(stash, name, 0); /* autoload: no */
	DBG_METHODCACHE(fprintf(stderr, "fetch cvname=%s\n", name));
      }
      if (gv) {
	/* if the method is already defined, get its cv. */
	cv = GvCV(gv);
	sv_cref_or_name = (SV *)cv;
        SvREFCNT_inc(sv_cref_or_name);
      } else {
	gv = gv_fetchmethod_autoload(stash, name, 1); /* autoload: yes */
	if (gv) {
	  /* if the method is autoloaded, get its name instead of cv. */
	  DBG_METHODCACHE(fprintf(stderr, "fetch svname=%s\n", name));
	  sv_cref_or_name = newSVpv(name, namelen);
	}
      }
    }
    if (sv_cref_or_name) {
      if (!pmc_plproxy_new_methodsv(aTHX_ ppl, cx, sv_cref_or_name, obj,
	vp)) {
	(*vp) = JSVAL_VOID;
	suc = JS_FALSE;
      } else {
	#if 0
	// FIXME: test caching behavior
	pmc_u16ch_t *u16str = NULL;
	size_t u16len = 0;
	u16str = JS_GetStringChars(jstr);
	u16len = JS_GetStringLength(jstr);
        suc = JS_DefineUCProperty(cx, obj, u16str, u16len, (*vp),
          NULL, NULL, JSPROP_READONLY);
	#endif
      }
      SvREFCNT_dec(sv_cref_or_name);
    }
    pmc_free(name, "pmc_plproxy_getprop");
  }
  return suc;
}

static JSBool
pmc_plproxy_setprop(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval id, jsval *vp)
{
  JSBool r;
  pmc_plpriv *pp;
  pmc_plpriv_calltype ct;

  DBG_SETPROP(pmc_dump_jsval(cx, "pmc_plproxy_setprop", id));
  pp = pmc_plproxy_get_plpriv(cx, obj);
  if (!pp) {
    return JS_TRUE;
  }
  ct = pmc_plpriv_get_calltype(pp);
  if (ct != pmc_plpriv_calltype_none) {
    /* obj is a package object */
    return pmc_plproxy_getsetpkg(aTHX_ ppl, cx, obj, pp, id, 1, vp);
  }
  r = pmc_plproxy_setcontainerelem(aTHX_ ppl, cx, obj, id, vp);
  return r;
}

static JSBool
pmc_plproxy_delprop(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  jsval id, jsval *vp)
{
  JSBool r;
  DBG_DELPROP(pmc_dump_jsval(cx, "pmc_plproxy_delprop", id));
  r = pmc_plproxy_delcontainerelem(aTHX_ ppl, cx, obj, id, vp);
  return r;
}

static JSBool
pmc_plproxy_convert(pTHX_ JSContext *cx, JSObject *obj, JSType ty,
  jsval *rval)
{
  pmc_plpriv *pp;
  SV *sv;

  DBG_CONV(fprintf(stderr, "conv %d\n", ty));
  pp = pmc_plproxy_get_plpriv(cx, obj);
  if (!pp) {
    DBG_CONV(fprintf(stderr, "conv2\n"));
    return pmc_plproxy_tostring_simple(cx, rval);
  }
  sv = pmc_plpriv_get_sv(pp);
  if (!sv) {
    DBG_CONV(fprintf(stderr, "conv3\n"));
    return pmc_plproxy_tostring_simple(cx, rval);
  }
  if (SvIOKp(sv)) {
    DBG_CONV(fprintf(stderr, "conv4\n"));
    if (ty == JSTYPE_NUMBER || ty == JSTYPE_VOID) {
      return pmc_sv_to_int(cx, SvIV(sv), rval);
    }
  } else if (SvNOKp(sv)) {
    DBG_CONV(fprintf(stderr, "conv5\n"));
    if (ty == JSTYPE_NUMBER || ty == JSTYPE_VOID) {
      return pmc_sv_to_double(cx, SvNV(sv), rval);
    }
  } else if (SvPOKp(sv)) {
    DBG_CONV(fprintf(stderr, "conv6\n"));
    if (ty == JSTYPE_STRING || ty == JSTYPE_VOID) {
      return pmc_sv_to_string(aTHX_ cx, pmc_tojs_mode_auto, sv, rval);
    }
  }
  DBG_CONV(fprintf(stderr, "conv7\n"));
  return pmc_plproxy_tostring_simple(cx, rval);
}

static JSBool
pmc_plproxy_enumerate_av(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  pmc_plpriv *pp, AV *av)
{
  JSBool suc = JS_TRUE;
  I32 arrlen = 0, i = 0;
  jsval alval = JSVAL_VOID;

  if (!pmc_plproxy_container_op_do(aTHX_ ppl, cx, obj, JSVAL_VOID,
    pmc_plproxy_container_op_fetchsize, &alval)) {
    return JS_FALSE;
  }
  if (!JSVAL_IS_INT(alval)) {
    return pmc_throw_js_logic_error(cx, "TypeMismatch",
      "pmc_plproxy_enumerate_av", "fetchsize returns an invalid value");
  }
  arrlen = av_len(av) + 1;
  for (i = 0; i < arrlen; ++i) {
    jsval jv = JSVAL_VOID;
    suc = JS_GetElement(cx, obj, i, &jv);
    if (suc) {
      suc = JS_SetElement(cx, obj, i, &jv);
    }
    if (!suc) {
      return pmc_throw_js_logic_error(cx, "PropertyNotFound",
	"pmc_plproxy_enumerate_av", "failed to define property");
    }
  }
  return suc;
}

static JSBool
pmc_plproxy_enumerate_hv(pTHX_ JSContext *cx, JSObject *obj, pmc_plpriv *pp)
{
  int x = 0;
  JSBool suc = JS_TRUE;
  HV *hv = NULL;
  HE *he = NULL;

  x = pmc_plpriv_iterator_first(pp);
  if (x != 0) {
    return JS_TRUE;
  }
  while (1) {
    SV *keysv = NULL;
    const char *key = NULL;
    STRLEN keylen = 0;
    int is_utf8 = 0;
    x = pmc_plpriv_iterator_next(pp, &hv, &he);
    if (x <= 0) {
      break;
    }
    if (hv == NULL || he == NULL) {
      continue;
    }
    keysv = hv_iterkeysv(he);
    if (!keysv || !SvPOK(keysv)) {
      continue;
    }
    is_utf8 = SvUTF8(keysv);
    key = SvPV(keysv, keylen);
    suc = pmc_utf_defineproperty(cx, obj, key, keylen, is_utf8, JSVAL_VOID,
      NULL, NULL, JSPROP_ENUMERATE);
    if (!suc) {
      return JS_FALSE;
    }
  }
  pmc_plpriv_iterator_next(pp, &hv, &he); /* finalize iter */
  return suc;
}

static JSBool
pmc_plproxy_enumerate(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj)
{
  pmc_plpriv *pp;
  SV *sv, *svtarget;

  pp = pmc_plproxy_get_plpriv(cx, obj);
  if (!pp) {
    return JS_TRUE;
  }
  sv = pmc_plpriv_get_sv(pp);
  if (!sv || !SvROK(sv)) {
    return JS_TRUE;
  }
  svtarget = SvRV(sv);
  JS_ClearScope(cx, obj);
  if (SvTYPE(svtarget) == SVt_PVAV) {
    return pmc_plproxy_enumerate_av(aTHX_ ppl, cx, obj, pp, (AV *)svtarget);
  } else {
    return pmc_plproxy_enumerate_hv(aTHX_ cx, obj, pp);
  }
}

static JSBool
pmc_plproxy_callsv(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj,
  pmc_plpriv *pp, uintN argc, jsval *argv, jsval *rval)
{
  SV *sv, *svtarget;
  int rvalc = 0;
  SV **rvalv = NULL;
  JSBool suc = JS_TRUE;
  const char *funcname;
 
  sv = pmc_plpriv_get_sv(pp);
  if (!sv || !SvROK(sv)) {
    pmc_throw_js_logic_error(cx, "NotFunction", "pmc_plproxy_callsv",
      "not a function");
    suc = JS_FALSE;
    goto err;
  }
  funcname = pmc_plpriv_get_function(pp);
  if (funcname) {
    /* TODO: this clause is not used anymore. */
    uintN iargc, i;
    jsval *iargv;
    /* abort(); */
    iargc = argc + 1;
    iargv = (jsval *)pmc_malloc(cx, iargc * sizeof(jsval),
      "pmc_plproxy_callsv");
    if (!iargv) {
      suc = JS_FALSE;
      goto err;
    }
    iargv[0] = OBJECT_TO_JSVAL(obj);
    for (i = 0; i < argc; ++i) {
      iargv[i + 1] = argv[i];
    }
    suc = pmc_funccall_scalarctx(aTHX_ ppl, cx, obj, iargc, iargv, rval,
      funcname, 1);
    pmc_free(iargv, "pmc_plproxy_callsv");
  } else {
    svtarget = SvRV(sv);
    if (!svtarget || SvTYPE(svtarget) != SVt_PVCV) {
      pmc_throw_js_logic_error(cx, "NotFunction", "pmc_plproxy_callsv",
	"not a function");
      goto err;
    }
    DBG_CALLSV(fprintf(stderr, "callsv\n"));
    suc = pmc_funccall_sv_argconv(aTHX_ ppl, cx, obj, argc, argv, &rvalc,
      &rvalv, sv, G_SCALAR);
    if (suc) {
      if (rvalc == 1 && rvalv[0] != NULL) {
	suc = pmc_sv_to_jsval(aTHX_ ppl, cx,
	  pmc_plproxy_get_tojs_current_mode(ppl), rvalv[0], rval);
      } else {
	pmc_throw_js_logic_error(cx, "InternalError",
	  "pmc_plproxy_callsv", "internal error");
	suc = JS_FALSE;
      }
    }
    pmc_free_svarr(aTHX_ rvalc, rvalv, "pmc_plproxy_callsv");
  }
  return suc;

 err:
  (*rval) = JSVAL_VOID;
  return suc;
}

static JSBool
pmc_plproxy_call_methodsv(pTHX_ PMCPerl *ppl, JSContext *cx,
  JSObject *obj, pmc_plpriv *pp, uintN argc, jsval *argv, jsval *rval)
{
  JSObject *parent;
  SV *svcref;
  uintN iargc, i;
  jsval *iargv;
  JSBool suc = JS_TRUE;

  svcref = pmc_plpriv_get_sv(pp);
  parent = JS_GetParent(cx, obj);
  if (!parent || !svcref) {
    (*rval) = JSVAL_VOID;
    return pmc_throw_js_logic_error(cx, "NotFunction",
      "pmc_plproxy_call_methodsv", "not a function");
  }
  iargc = argc + 1;
  iargv = (jsval *)pmc_malloc(cx, iargc * sizeof(jsval),
    "pmc_plproxy_call_methodsv");
  if (!iargv) {
    (*rval) = JSVAL_VOID;
    return JS_FALSE;
  }
  iargv[0] = OBJECT_TO_JSVAL(parent);
  for (i = 0; i < argc; ++i) {
    iargv[i + 1] = argv[i];
  }
  if (SvTYPE(svcref) == SVt_PVCV) {
    suc = pmc_funccall_sv_scalarctx(aTHX_ ppl, cx, parent, iargc, iargv, rval,
      svcref);
  } else if (SvTYPE(svcref) == SVt_PV) {
    suc = pmc_funccall_scalarctx(aTHX_ ppl, cx, parent, iargc, iargv, rval,
      SvPV_nolen(svcref), 1);
  }
  pmc_free(iargv, "pmc_plproxy_call_methodsv");
  return suc;
}

static JSBool
pmc_plproxy_call(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj, uintN argc,
  jsval *argv, jsval *rval)
{
  pmc_plpriv *pp;
  pmc_plpriv_calltype ct;
  const char *pkgname;
  const char *funcname;

#if 0
  /* i have no idea why argv[-2] is used. see regexp_call() in
    jsregexp.c . */
  if (!JSVAL_IS_OBJECT(argv[-2])) {
    (*rval) = JSVAL_VOID;
    return JS_TRUE;
  }
  obj = JSVAL_TO_OBJECT(argv[-2]); /* overwrite obj */
#endif
  pp = pmc_plproxy_get_plpriv(cx, obj);
  if (!pp) {
    (*rval) = JSVAL_VOID;
    return JS_TRUE;
  }
  ct = pmc_plpriv_get_calltype(pp);
  if (ct == pmc_plpriv_calltype_none) {
    return pmc_plproxy_callsv(aTHX_ ppl, cx, obj, pp, argc, argv, rval);
  } else if (ct == pmc_plpriv_calltype_methodsv) {
    return pmc_plproxy_call_methodsv(aTHX_ ppl, cx, obj, pp, argc, argv,
      rval);
  }
  pkgname = pmc_plpriv_get_package(pp);
  funcname = pmc_plpriv_get_function(pp);
  if (!pkgname || !funcname) {
    (*rval) = JSVAL_VOID;
    return JS_TRUE;
  }
  return pmc_plproxy_callpkg(aTHX_ ppl, cx, ct, pkgname, funcname, argc,
    argv, rval);
}

typedef JSBool (*pmc_plproxy_prop_func_type)(PerlInterpreter *pl,
  PMCPerl *ppl, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
typedef JSBool (*pmc_plproxy_cb_func_type)(PerlInterpreter *pl,
  PMCPerl *ppl, JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
  jsval *rval);

/* the ENTER+SAVETMPS / FREETMPS+LEAVE pairs are necessary for freeing
  perl temporal variables. */

static JSBool
pmc_plproxy_generic_prop(JSContext *cx, JSObject *obj, jsval id, jsval *vp,
  pmc_plproxy_prop_func_type func)
{
  PMCPerl *ppl;
  PerlInterpreter *my_perl;
  JSBool suc;
  JSContext *oldcx;
  ppl = pmc_plproxy_enter(cx, obj, &oldcx);
  my_perl = pmc_get_interpreter(ppl);
  if (!my_perl) {
    (*vp) = JSVAL_VOID;
    suc = pmc_throw_js_nopmc(cx, "pmc_plproxy_generic_prop");
    goto out;
  }
  ENTER;
  SAVETMPS;
  suc = (*func)(my_perl, ppl, cx, obj, id, vp);
  FREETMPS;
  LEAVE;
 out:
  pmc_plproxy_leave(ppl, oldcx);
  return suc;
}

static JSBool
pmc_plproxy_generic_cb(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
  jsval *rval, pmc_plproxy_cb_func_type func)
{
  PMCPerl *ppl;
  PerlInterpreter *my_perl;
  JSBool suc;
  JSContext *oldcx;
  ppl = pmc_plproxy_enter(cx, obj, &oldcx);
  my_perl = pmc_get_interpreter(ppl);
  if (!my_perl) {
    (*rval) = JSVAL_VOID;
    suc = pmc_throw_js_nopmc(cx, "pmc_plproxy_generic_cb");
    goto out;
  }
  ENTER;
  SAVETMPS;
  suc = (*func)(my_perl, ppl, cx, obj, argc, argv, rval);
  FREETMPS;
  LEAVE;
 out:
  pmc_plproxy_leave(ppl, oldcx);
  return suc;
}

#define PMC_DEF_PROPOP_WRAPPER(func) \
static JSBool \
pmc_plproxy_prop_ ## func(JSContext *cx, JSObject *obj, jsval id, jsval *vp) \
{ \
  return pmc_plproxy_generic_prop(cx, obj, id, vp, func); \
}

#define PMC_DEF_CB_WRAPPER(func) \
static JSBool \
pmc_plproxy_cb_ ## func(JSContext *cx, JSObject *obj, uintN argc, \
  jsval *argv, jsval *rval) \
{ \
  return pmc_plproxy_generic_cb(cx, obj, argc, argv, rval, func); \
}

PMC_DEF_CB_WRAPPER(pmc_plproxy_invoke)
PMC_DEF_CB_WRAPPER(pmc_plproxy_invokelc)
PMC_DEF_CB_WRAPPER(pmc_plproxy_plsetprop)
PMC_DEF_CB_WRAPPER(pmc_plproxy_plgetprop)
PMC_DEF_CB_WRAPPER(pmc_plproxy_pldelprop)
PMC_DEF_CB_WRAPPER(pmc_plproxy_setvalue)
PMC_DEF_CB_WRAPPER(pmc_plproxy_setbinaryvalue)
PMC_DEF_CB_WRAPPER(pmc_plproxy_getvalue)
PMC_DEF_CB_WRAPPER(pmc_plproxy_gettext)
PMC_DEF_CB_WRAPPER(pmc_plproxy_getbinary)
PMC_DEF_CB_WRAPPER(pmc_plproxy_tostring)
PMC_DEF_CB_WRAPPER(pmc_plproxy_detach)

static const char *pmc_plproxy_methods_for_object[] = {
  "Invoke",
  "InvokeLC",
  "SetProperty",
  "GetProperty",
  "DelProperty",
  "toString",
  "Detach",
  "__noSuchMethod__",
  NULL,
};

static JSFunctionSpec pmc_plproxy_methods[] = {
  { "Invoke", pmc_plproxy_cb_pmc_plproxy_invoke,            1, 0, 0 },
  { "InvokeLC", pmc_plproxy_cb_pmc_plproxy_invokelc,        1, 0, 0 },
  { "SetProperty", pmc_plproxy_cb_pmc_plproxy_plsetprop,    2, 0, 0 },
  { "GetProperty", pmc_plproxy_cb_pmc_plproxy_plgetprop,    1, 0, 0 },
  { "DelProperty", pmc_plproxy_cb_pmc_plproxy_pldelprop,    1, 0, 0 },
  { "Set", pmc_plproxy_cb_pmc_plproxy_setvalue,             1, 0, 0 },
  { "SetText", pmc_plproxy_cb_pmc_plproxy_setvalue,         1, 0, 0 },
  { "SetBinary", pmc_plproxy_cb_pmc_plproxy_setbinaryvalue, 1, 0, 0 },
  { "Get", pmc_plproxy_cb_pmc_plproxy_getvalue,             0, 0, 0 },
  { "GetText", pmc_plproxy_cb_pmc_plproxy_gettext,          0, 0, 0 },
  { "GetBinary", pmc_plproxy_cb_pmc_plproxy_getbinary,      0, 0, 0 },
  { "toString", pmc_plproxy_cb_pmc_plproxy_tostring,        0, 0, 0 },
  { "Detach", pmc_plproxy_cb_pmc_plproxy_detach,            0, 0, 0 },
  { NULL, NULL, 0, 0, 0 },
};

PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_getfunctions)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_getmethods)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_getconstants)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_getfunctionslc)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_getmethodslc)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_getconstantslc)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_getvariables)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_getarrays)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_gethashes)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_gethandles)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_getrawfunctions)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_setdummy)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_geterrno)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_seterrno)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_getarg)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_setarg)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_geta)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_seta)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_getb)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_setb)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_getchilderr)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_setchilderr)

static JSPropertySpec pmc_plproxy_static_props[] = {
  { "Functions", -3, 0, pmc_plproxy_prop_pmc_plproxy_getfunctions,
    pmc_plproxy_prop_pmc_plproxy_setdummy },
  { "Methods", -4, 0, pmc_plproxy_prop_pmc_plproxy_getmethods,
    pmc_plproxy_prop_pmc_plproxy_setdummy },
  { "Constants", -5, 0, pmc_plproxy_prop_pmc_plproxy_getconstants,
    pmc_plproxy_prop_pmc_plproxy_setdummy },
  { "FunctionsLC", -6, 0, pmc_plproxy_prop_pmc_plproxy_getfunctionslc,
    pmc_plproxy_prop_pmc_plproxy_setdummy },
  { "MethodsLC", -7, 0, pmc_plproxy_prop_pmc_plproxy_getmethodslc,
    pmc_plproxy_prop_pmc_plproxy_setdummy },
  { "ConstantsLC", -8, 0, pmc_plproxy_prop_pmc_plproxy_getconstantslc,
    pmc_plproxy_prop_pmc_plproxy_setdummy },
  { "Scalars", -10, 0, pmc_plproxy_prop_pmc_plproxy_getvariables,
    pmc_plproxy_prop_pmc_plproxy_setdummy },
  { "Arrays", -11, 0, pmc_plproxy_prop_pmc_plproxy_getarrays,
    pmc_plproxy_prop_pmc_plproxy_setdummy },
  { "Hashes", -12, 0, pmc_plproxy_prop_pmc_plproxy_gethashes,
    pmc_plproxy_prop_pmc_plproxy_setdummy },
  { "Handles", -13, 0, pmc_plproxy_prop_pmc_plproxy_gethandles,
    pmc_plproxy_prop_pmc_plproxy_setdummy },
  { "FunctionsRaw", -14, 0, pmc_plproxy_prop_pmc_plproxy_getrawfunctions,
    pmc_plproxy_prop_pmc_plproxy_setdummy },
  { "ERRNO", -15, 0, pmc_plproxy_prop_pmc_plproxy_geterrno,
    pmc_plproxy_prop_pmc_plproxy_seterrno },
  { "ARG", -16, 0, pmc_plproxy_prop_pmc_plproxy_getarg,
    pmc_plproxy_prop_pmc_plproxy_setarg },
  { "a", -17, 0, pmc_plproxy_prop_pmc_plproxy_geta,
    pmc_plproxy_prop_pmc_plproxy_seta },
  { "b", -18, 0, pmc_plproxy_prop_pmc_plproxy_getb,
    pmc_plproxy_prop_pmc_plproxy_setb },
  { "CHILD_ERROR", -19, 0, pmc_plproxy_prop_pmc_plproxy_getchilderr,
    pmc_plproxy_prop_pmc_plproxy_setchilderr },
  { NULL, 0, 0, NULL, NULL }, 
};

PMC_DEF_CB_WRAPPER(pmc_plproxy_use)
PMC_DEF_CB_WRAPPER(pmc_plproxy_eval)
PMC_DEF_CB_WRAPPER(pmc_plproxy_suppressconv)
PMC_DEF_CB_WRAPPER(pmc_plproxy_newobject)
PMC_DEF_CB_WRAPPER(pmc_plproxy_newscalar)
PMC_DEF_CB_WRAPPER(pmc_plproxy_newarray)
PMC_DEF_CB_WRAPPER(pmc_plproxy_newhash)
PMC_DEF_CB_WRAPPER(pmc_plproxy_isperl)
PMC_DEF_CB_WRAPPER(pmc_plproxy_tie)
PMC_DEF_CB_WRAPPER(pmc_plproxy_untie)
PMC_DEF_CB_WRAPPER(pmc_plproxy_coercetofunction)
PMC_DEF_CB_WRAPPER(pmc_plproxy_newtiedscalar)
PMC_DEF_CB_WRAPPER(pmc_plproxy_newtiedarray)
PMC_DEF_CB_WRAPPER(pmc_plproxy_newtiedhash)
PMC_DEF_CB_WRAPPER(pmc_plproxy_calljs)
PMC_DEF_CB_WRAPPER(pmc_plproxy_newjs)
PMC_DEF_CB_WRAPPER(pmc_plproxy_bintotext)
PMC_DEF_CB_WRAPPER(pmc_plproxy_texttobin)
PMC_DEF_CB_WRAPPER(pmc_plproxy_detachperlproxies)
PMC_DEF_CB_WRAPPER(pmc_plproxy_detachjsproxies)
PMC_DEF_CB_WRAPPER(pmc_plproxy_detachproxies)
PMC_DEF_CB_WRAPPER(pmc_plproxy_detachone)

static JSFunctionSpec pmc_plproxy_static_funcs[] = {
  { "Use", pmc_plproxy_cb_pmc_plproxy_use,                         1, 0, 0 },
  { "Eval", pmc_plproxy_cb_pmc_plproxy_eval,                       2, 0, 0 },
  { "EvalString", pmc_plproxy_cb_pmc_plproxy_eval,                 2, 0, 0 },
    /* used from ASE */
  { "SuppressConv", pmc_plproxy_cb_pmc_plproxy_suppressconv,       1, 0, 0 },
  { "New", pmc_plproxy_cb_pmc_plproxy_newobject,                   1, 0, 0 },
  { "NewInstance", pmc_plproxy_cb_pmc_plproxy_newobject,           1, 0, 0 },
    /* used from ASE */
  { "NewScalar", pmc_plproxy_cb_pmc_plproxy_newscalar,             0, 0, 0 },
  { "NewArray", pmc_plproxy_cb_pmc_plproxy_newarray,               0, 0, 0 },
  { "NewHash", pmc_plproxy_cb_pmc_plproxy_newhash,                 0, 0, 0 },
  { "IsPerl", pmc_plproxy_cb_pmc_plproxy_isperl,                   1, 0, 0 },
  { "Tie", pmc_plproxy_cb_pmc_plproxy_tie,                         2, 0, 0 },
  { "Untie", pmc_plproxy_cb_pmc_plproxy_untie,                     1, 0, 0 },
  { "CoerceToFunction", pmc_plproxy_cb_pmc_plproxy_coercetofunction,
                                                                   1, 0, 0 },
  { "NewTiedScalar", pmc_plproxy_cb_pmc_plproxy_newtiedscalar,     1, 0, 0 },
  { "NewTiedArray", pmc_plproxy_cb_pmc_plproxy_newtiedarray,       1, 0, 0 },
  { "NewTiedHash", pmc_plproxy_cb_pmc_plproxy_newtiedhash,         1, 0, 0 },
  { "CallJS", pmc_plproxy_cb_pmc_plproxy_calljs,                   1, 0, 0 },
  { "NewJS", pmc_plproxy_cb_pmc_plproxy_newjs,                     1, 0, 0 },
  { "BinToText", pmc_plproxy_cb_pmc_plproxy_bintotext,             1, 0, 0 },
  { "TextToBin", pmc_plproxy_cb_pmc_plproxy_texttobin,             1, 0, 0 },
  { "DetachPerlProxies", pmc_plproxy_cb_pmc_plproxy_detachperlproxies,
                                                                   0, 0, 0 },
  { "DetachJSProxies", pmc_plproxy_cb_pmc_plproxy_detachjsproxies, 0, 0, 0 },
  { "DetachProxies", pmc_plproxy_cb_pmc_plproxy_detachproxies,     0, 0, 0 },
  { "Detach", pmc_plproxy_cb_pmc_plproxy_detachone,                0, 0, 0 },
  { "Apply", pmc_plproxy_apply,                                    2, 0, 0 },
  { NULL, NULL, 0, 0, 0 },
};

PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_getlength)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_setlength)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_getproto)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_setproto)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_getparent)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_setparent)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_getcount)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_setcount)

static JSPropertySpec pmc_plproxy_props[] = {
  { "length", -10, 0, pmc_plproxy_prop_pmc_plproxy_getlength,
    pmc_plproxy_prop_pmc_plproxy_setlength },
  /* hide pre-defined properies */
  { "__proto__", -11, 0, pmc_plproxy_prop_pmc_plproxy_getproto,
    pmc_plproxy_prop_pmc_plproxy_setproto },
  { "__parent__", -12, 0, pmc_plproxy_prop_pmc_plproxy_getparent,
    pmc_plproxy_prop_pmc_plproxy_setparent },
  { "__count__", -13, 0, pmc_plproxy_prop_pmc_plproxy_getcount,
    pmc_plproxy_prop_pmc_plproxy_setcount },
  { NULL, 0, 0, NULL, NULL }, 
};

PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_delprop)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_getprop)
PMC_DEF_PROPOP_WRAPPER(pmc_plproxy_setprop)

static JSBool
pmc_plproxy_wrap_pmc_plproxy_convert(JSContext *cx, JSObject *obj, JSType ty,
  jsval *rval)
{
  PMCPerl *ppl;
  PerlInterpreter *my_perl;
  JSBool suc;
  JSContext *oldcx;
  if (ty == JSTYPE_FUNCTION) {
    /* called before plproxy_call is called. return immediately in order to
      make method invocations faster. */
    (*rval) = JSVAL_VOID;
    return JS_TRUE;
  }
  ppl = pmc_plproxy_enter(cx, obj, &oldcx);
  my_perl = pmc_get_interpreter(ppl);
  if (!my_perl) {
    (*rval) = JSVAL_VOID;
    suc = pmc_throw_js_nopmc(cx, "pmc_plproxy_wrap_pmc_plproxy_convert");
    goto out;
  }
  ENTER;
  SAVETMPS;
  suc = pmc_plproxy_convert(my_perl, cx, obj, ty, rval);
  FREETMPS;
  LEAVE;
 out:
  pmc_plproxy_leave(ppl, oldcx);
  return suc;
}

static JSBool
pmc_plproxy_wrap_pmc_plproxy_call(JSContext *cx, JSObject *obj, uintN argc,
  jsval *argv, jsval *rval)
{
  PMCPerl *ppl;
  PerlInterpreter *my_perl;
  JSBool suc;
  JSContext *oldcx;
  /* argv[-2] is the perl proxy to be called */
  if (!JSVAL_IS_OBJECT(argv[-2])) {
    (*rval) = JSVAL_VOID;
    return JS_TRUE;
  }
  obj = JSVAL_TO_OBJECT(argv[-2]); /* overwrite obj */
  ppl = pmc_plproxy_enter(cx, obj, &oldcx);
  my_perl = pmc_get_interpreter(ppl);
  if (!my_perl) {
    (*rval) = JSVAL_VOID;
    suc = pmc_throw_js_nopmc(cx, "pmc_plproxy_wrap_pmc_plproxy_call");
    goto out;
  }
  ENTER;
  SAVETMPS;
  suc = pmc_plproxy_call(my_perl, ppl, cx, obj, argc, argv, rval);
  FREETMPS;
  LEAVE;
 out:
  pmc_plproxy_leave(ppl, oldcx);
  return suc;
}

static JSBool
pmc_plproxy_wrap_pmc_plproxy_enumerate(JSContext *cx, JSObject *obj)
{
  PMCPerl *ppl;
  PerlInterpreter *my_perl;
  JSBool suc;
  JSContext *oldcx;
  ppl = pmc_plproxy_enter(cx, obj, &oldcx);
  my_perl = pmc_get_interpreter(ppl);
  if (!my_perl) {
    suc = pmc_throw_js_nopmc(cx, "pmc_plproxy_wrap_pmc_plproxy_enumerate");
    goto out;
  }
  ENTER;
  SAVETMPS;
  suc = pmc_plproxy_enumerate(my_perl, ppl, cx, obj);
  FREETMPS;
  LEAVE;
 out:
  pmc_plproxy_leave(ppl, oldcx);
  return suc;
}

JSClass pmc_perl_class = {
  "Perl", JSCLASS_HAS_PRIVATE,
  JS_PropertyStub, pmc_plproxy_prop_pmc_plproxy_delprop,
  pmc_plproxy_prop_pmc_plproxy_getprop,
  pmc_plproxy_prop_pmc_plproxy_setprop,
  pmc_plproxy_wrap_pmc_plproxy_enumerate, JS_ResolveStub,
  // pmc_plproxy_wrap_pmc_plproxy_convert, pmc_plproxy_finalize,
    // FIXME: remove
  pmc_plproxy_wrap_pmc_plproxy_convert, pmc_plproxy_finalize_noperllock,
  NULL, NULL, pmc_plproxy_wrap_pmc_plproxy_call, NULL, NULL, NULL, NULL, NULL
};


PMC_DEF_CB_WRAPPER(pmc_plproxy_constr)

JSObject *
pmc_init_perl_class(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *obj)
{
  JSObject *proto;
  DBG(fprintf(stderr, "InitPerlClass\n"));
  proto = JS_InitClass(cx, obj, NULL, &pmc_perl_class,
    pmc_plproxy_cb_pmc_plproxy_constr, 1, pmc_plproxy_props,
    pmc_plproxy_methods, pmc_plproxy_static_props, pmc_plproxy_static_funcs);
  DBG_PROTO(fprintf(stderr, "PROTO=%p CLASS=%p\n", proto, &pmc_perl_class));
  DBG_PROTO(fprintf(stderr, "INI OBJ=%p\n", obj));
  if (!pmc_plproxy_attach_plpriv(aTHX_ ppl, cx, proto, NULL)) {
    return NULL;
  }
  return proto;
}

