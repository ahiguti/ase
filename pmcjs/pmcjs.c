
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include "pmcjs.h"
#include "pmcjs_impl.h"
#include "pmobject.h"
#include "pmcexception.h"
#include "pmconv.h"
#include "polist.h"
#include "pcondemand.h"

#include <prmon.h>
#include <prlock.h>

#ifdef _MSC_VER
#include <delayimp.h>
#endif

#define DBG_DELAY(x)
#define DBG_API(x)
#define DBG_LIST(x)
#define DBG_PPX(x)
#define DBG_JSGL(x)
#define DBG_MOD(x)
#define DBG_THREAD(x)

#undef NODELAY_JS_IN_PERL
#undef LEAK_MEM_ONE

#ifdef _MSC_VER

static int
pmc_exc_filter(EXCEPTION_POINTERS *ptrs)
{
  if (ptrs && ptrs->ExceptionRecord) {
    DWORD code;
    code = ptrs->ExceptionRecord[0].ExceptionCode;
    if (code == VcppException(ERROR_SEVERITY_ERROR, ERROR_MOD_NOT_FOUND) ||
        code == VcppException(ERROR_SEVERITY_ERROR, ERROR_PROC_NOT_FOUND)) {
      DBG_DELAY(fprintf(stderr, "perl not found\n"));
      return EXCEPTION_EXECUTE_HANDLER;
    }
  }
  return EXCEPTION_CONTINUE_EXECUTION;
}

int
PMC_PerlAvailable()
{
  {
    int r = 0;
    __try {
      r = SUCCEEDED(__HrLoadAllImportsForDll("perl510.dll"));
    } __except(pmc_exc_filter(GetExceptionInformation())) {
      return 0;
    }
    return r;
  }
}

#else

int
PMC_PerlAvailable()
{
  return 1;
}

#endif

static PMC_APIFunctionTable_2 pmc_api_function_table = {
  PMC_PerlAvailable,
  PMC_Init,
  PMC_Terminate,
  PMC_NewPerl,
  PMC_DestroyPerl,
  PMC_InitClasses,
  PMC_SetArgv,
  PMC_SetContextThread,
  PMC_ClearContextThread,
  PMC_Eval,
  PMC_EvalFile,
  PMC_Require,
  PMC_DetachProxies,
  PMC_DetachJSProxies,
  PMC_DetachPerlProxies,
};

PMC_APIFunctionTable_2 *
PMC_GetAPIFunctionTable_2()
{
  return &pmc_api_function_table;
}

void
PMC_Init(int argc, char **argv, char **env)
{
  DBG_API(fprintf(stderr, "PMC_Init\n"));
#if 0
  /* see perlmain.c */
  #if !defined(PERL_USE_SAFE_PUTENV) && !defined(_MSC_VER)
  PL_use_safe_putenv = 0;
  #endif
#endif
  PERL_SYS_INIT3(&argc, &argv, &env);
}

void /* Obsolete */
PMC_SetCallback(PMCPerl *(*ptr)(JSContext *cx))
{
  DBG_API(fprintf(stderr, "PMC_SetCallback\n"));
}

void
PMC_Terminate()
{
  DBG_API(fprintf(stderr, "PMC_Terminate\n"));
  PERL_SYS_TERM();
}

const char *
PMC_Version()
{
  return "$Rev: 5254 $";
}

void boot_DynaLoader(pTHX_ CV *cv);
void boot_JavaScript(pTHX_ CV *cv);
void XS_JavaScript_LoadModule(pTHX_ CV *cv);
void XS_JavaScript_DESTROY(pTHX_ CV *cv);
void XS_JavaScript_NewPassive(pTHX_ CV *cv);
void XS_JavaScript_new(pTHX_ CV *cv);
void XS_JavaScript_Call(pTHX_ CV *cv);
void XS_JavaScript_GetProperty(pTHX_ CV *cv);
void XS_JavaScript_GetElement(pTHX_ CV *cv);
void XS_JavaScript_SetProperty(pTHX_ CV *cv);
void XS_JavaScript_SetElement(pTHX_ CV *cv);
void XS_JavaScript_DelProperty(pTHX_ CV *cv);
void XS_JavaScript_DelElement(pTHX_ CV *cv);
void XS_JavaScript_GetLength(pTHX_ CV *cv);
void XS_JavaScript_SetLength(pTHX_ CV *cv);
void XS_JavaScript_ResetIterator(pTHX_ CV *cv);
void XS_JavaScript_Iterate(pTHX_ CV *cv);
void XS_JavaScript_Invoke(pTHX_ CV *cv);
void XS_JavaScript_InvokeSelf(pTHX_ CV *cv);
void XS_JavaScript_Eval(pTHX_ CV *cv);

static void
pmc_xs_init(pTHX)
{
  char *file = __FILE__;
  newXS("DynaLoader::boot_DynaLoader", boot_DynaLoader, file);
  newXS("JavaScript::boot_JavaScript", boot_JavaScript, file);
  newXS("JavaScript::LoadModule", XS_JavaScript_LoadModule, file);
  newXS("JavaScript::DESTROY", XS_JavaScript_DESTROY, file);
  newXS("JavaScript::NewPassive", XS_JavaScript_NewPassive, file);
  newXS("JavaScript::new", XS_JavaScript_new, file);
  newXS("JavaScript::Call", XS_JavaScript_Call, file);
  newXS("JavaScript::GetProperty", XS_JavaScript_GetProperty, file);
  newXS("JavaScript::GetElement", XS_JavaScript_GetElement, file);
  newXS("JavaScript::SetProperty", XS_JavaScript_SetProperty, file);
  newXS("JavaScript::SetElement", XS_JavaScript_SetElement, file);
  newXS("JavaScript::DelProperty", XS_JavaScript_DelProperty, file);
  newXS("JavaScript::DelElement", XS_JavaScript_DelElement, file);
  newXS("JavaScript::GetLength", XS_JavaScript_GetLength, file);
  newXS("JavaScript::SetLength", XS_JavaScript_SetLength, file);
  newXS("JavaScript::ResetIterator", XS_JavaScript_ResetIterator, file);
  newXS("JavaScript::Iterate", XS_JavaScript_Iterate, file);
  newXS("JavaScript::Invoke", XS_JavaScript_Invoke, file);
  newXS("JavaScript::InvokeSelf", XS_JavaScript_InvokeSelf, file);
  newXS("JavaScript::Eval", XS_JavaScript_Eval, file);
}

struct PMCPerl_s {
  PerlInterpreter *interp;
  PRMonitor *monitor;
  PRLock *pmo_lock;
  JSContext *current_jsctx;
  pmc_list *pjs_list;
  pmc_list *pmo_list;
  pmc_list *pmo_finalized_list;
  pmc_jsglobal *pjg;
  HV *loaded_modules;
  int js_load : 8;
  int suppress_conv : 1;
};

enum {
  js_load_util = 1,
  js_load_objwrap = 2,
  js_load_core = 4,
};

#ifdef LEAK_MEM_ONE
static void __attribute__((noinline))
leak_mem_one()
{
  /* dummy for leak detector */
  malloc(111);
}
#endif

static JSBool
pmc_eval_raw_internal(pTHX_ JSContext *cx, const char *str)
{
  JSBool suc = JS_TRUE;
  ENTER;
  SAVETMPS;
  eval_pv(str, 0); /* ignore syntax error */
  if (SvTRUE(ERRSV)) {
    char *estr;
    STRLEN elen;
    estr = SvPV(ERRSV, elen);
    pmc_throw_js_runtime_exception(cx, "PerlException", "pmc_eval_internal",
      estr);
    suc = JS_FALSE;
  }
  FREETMPS;
  LEAVE;
  return suc;
}

JSBool
pmc_eval_internal(pTHX_ PMCPerl *ppl, JSContext *cx, const char *str,
  const char *filename /* nullable */, jsval *rv /* nullable */)
{
  JSBool suc = JS_TRUE;
  SV *strpv;
  int rvalc = 0;
  SV **rvalv = NULL;

  ENTER;
  SAVETMPS;
  
  /* require JavaScript::Util */
  pmc_load_javascript_util(ppl, cx);

  if (rv) {
    (*rv) = JSVAL_VOID;
  }

  strpv = newSVpv(str, 0);;
  suc = pmc_funccall_internal(aTHX_ cx, 1, &strpv, &rvalc, &rvalv,
    "JavaScript::Util::eval_str", 0, G_ARRAY);
  if (suc && rvalc == 2) {
    if (SvPOK(rvalv[0])) {
      char *s = NULL;
      STRLEN slen = 0;
      s = SvPV(rvalv[0], slen);
      if (slen != 0) {
	int fnlen;
	int mlen;
	char *buf = NULL;
	fnlen = filename ? strlen(filename) : 0;
	mlen = slen + 5 + fnlen + 1;
	buf = (char *)pmc_malloc(cx, mlen, "pmc_eval_internal");
	if (buf) {
	  memcpy(buf, s, slen);
	  if (filename) {
	    memcpy(buf + slen, "file=", 5);
	    memcpy(buf + slen + 5, filename, fnlen);
	    buf[mlen - 1] = 0;
	  } else {
	    buf[slen] = 0;
	  }
	  pmc_throw_js_runtime_exception(cx, "PerlException",
	    "pmc_eval_internal", buf);
	  pmc_free(buf, "pmc_eval_internal");
	}
	suc = JS_FALSE;
      }
    }
    if (suc && rv) {
      suc = pmc_sv_to_jsval(aTHX_ ppl, cx, pmc_tojs_mode_auto, rvalv[1], rv);
    }
  }
  pmc_free_svarr(aTHX_ rvalc, rvalv, "pmc_eval_internal");
  SvREFCNT_dec(strpv);

  FREETMPS;
  LEAVE;

  return suc;
}

static JSBool
pmc_require_internal(pTHX_ JSContext *cx, const char *str)
{
  JSBool suc = JS_TRUE;
  ENTER;
  SAVETMPS;
  require_pv(str);
  if (SvTRUE(ERRSV)) {
    char *estr;
    STRLEN elen;
    estr = SvPV(ERRSV, elen);
    if (cx) {
      pmc_throw_js_runtime_exception(cx, "PerlException",
	"pmc_require_internal", estr);
    }
    suc = JS_FALSE;
  }
  FREETMPS;
  LEAVE;
  return suc;
}

JSBool
PMC_Require(PMCPerl *ppl, JSContext *cx, const char *filename)
{
  PerlInterpreter *my_perl;
  JSContext *oldcx;
  JSBool r = JS_FALSE;
  my_perl = pmc_get_interpreter(ppl);
  if (!my_perl) {
    return JS_FALSE;
  }
  oldcx = pmc_enter_pmcperl(ppl, cx);
  r = pmc_require_internal(aTHX_ cx, filename);
  pmc_leave_pmcperl(ppl, cx);
  return r;
}

JSBool
PMC_Eval(PMCPerl *ppl, JSContext *cx, const char *str)
{
  return PMC_EvalFile(ppl, cx, str, NULL);
}

JSBool
PMC_EvalFile(PMCPerl *ppl, JSContext *cx, const char *str,
  const char *filename)
{
  PerlInterpreter *my_perl;
  JSContext *oldcx;
  JSBool r = JS_FALSE;
  my_perl = pmc_get_interpreter(ppl);
  if (!my_perl) {
    return JS_FALSE;
  }
  oldcx = pmc_enter_pmcperl(ppl, cx);
  r = pmc_eval_internal(aTHX_ ppl, cx, str, filename, NULL);
  pmc_leave_pmcperl(ppl, cx);
  return r;
}

static void
pmc_load_javascript_internal(PMCPerl *ppl, JSContext *cx, unsigned int mask,
  const char *str)
{
  PerlInterpreter *my_perl;
  if ((ppl->js_load & mask) != 0) {
    return;
  }
  my_perl = pmc_get_interpreter(ppl);
  if (!my_perl) {
    return;
  }
  pmc_eval_raw_internal(aTHX_ cx, str);
  ppl->js_load |= mask;
}

void
pmc_load_javascript_util(PMCPerl *ppl, JSContext *cx)
{
  pmc_load_javascript_internal(ppl, cx, js_load_util,
    pmc_get_pkg_javascript_util_str());
}

void
pmc_load_javascript_objwrap(PMCPerl *ppl, JSContext *cx)
{
  pmc_load_javascript_internal(ppl, cx, js_load_objwrap,
    pmc_get_pkg_javascript_objwrap_str());
}

void
pmc_load_javascript_core(PMCPerl *ppl, JSContext *cx)
{
  pmc_load_javascript_internal(ppl, cx, js_load_core,
    pmc_get_pkg_javascript_core_str());
}

PMCPerl *
PMC_NewPerl()
{
  PerlInterpreter *my_perl;
  char *embedding[] = { "pmcjs", "-e", "1" };
  PMCPerl *ppl;
  ppl = (PMCPerl *)pmc_malloc_nocx(sizeof(*ppl), "PMC_NewPerl");
  if (!ppl) {
    goto err;
  }
  DBG_API(fprintf(stderr, "PMC_NewPerl %p\n", ppl));
  ppl->interp = NULL;
  ppl->monitor = NULL;
  ppl->pmo_lock = NULL;
  ppl->current_jsctx = NULL;
  ppl->pjs_list = NULL;
  ppl->pmo_list = NULL;
  ppl->pmo_finalized_list = NULL;
  ppl->pjg = NULL;
  ppl->loaded_modules = NULL;
  ppl->pjs_list = pmc_list_new("pmc_newperl_pjs");
  ppl->js_load = 0;
  ppl->suppress_conv = 0;
  if (!ppl->pjs_list) {
    goto err;
  }
  ppl->pmo_list = pmc_list_new("pmc_newperl_pmo");
  if (!ppl->pmo_list) {
    goto err;
  }
  ppl->pmo_finalized_list = pmc_list_new("pmc_newperl_pmofin");
  if (!ppl->pmo_finalized_list) {
    goto err;
  }
  ppl->monitor = PR_NewMonitor();
  if (!ppl->monitor) {
    goto err;
  }
  ppl->pmo_lock = PR_NewLock();
  if (!ppl->pmo_lock) {
    goto err;
  }
  my_perl = perl_alloc();
  if (!my_perl) {
    goto err;
  }
  perl_construct(aTHX);
  ppl->interp = my_perl;

  ppl->loaded_modules = newHV();

  PL_exit_flags |= PERL_EXIT_DESTRUCT_END;
  perl_parse(aTHX_ pmc_xs_init, 3, embedding, (char **)NULL);

  #ifdef NODELAY_JS_IN_PERL
  pmc_load_javascript_internal(ppl, NULL, js_load_util,
    pmc_get_pkg_javascript_util_str());
  pmc_load_javascript_internal(ppl, NULL, js_load_objwrap,
    pmc_get_pkg_javascript_objwrap_str());
  pmc_load_javascript_internal(ppl, NULL, js_load_core,
    pmc_get_pkg_javascript_core_str());
  #endif

  #ifdef LEAK_MEM_ONE
  leak_mem_one();
  #endif

  return ppl;

 err:

  PMC_DestroyPerl(ppl, NULL);
  return NULL;
}

void
PMC_DetachProxies(PMCPerl *ppl, JSContext *cx)
{
  JSContext *oldcx;
  oldcx = pmc_enter_pmcperl(ppl, cx);
  PMC_DetachJSProxies(ppl, cx);
  PMC_DetachPerlProxies(ppl, cx);
  pmc_leave_pmcperl(ppl, oldcx);
}

void
PMC_DetachJSProxies(PMCPerl *ppl, JSContext *cx)
{
  if (ppl && cx) {
    JSContext *oldcx;
    oldcx = pmc_enter_pmcperl(ppl, cx);
    pmc_detach_js_proxies(ppl, cx);
    pmc_leave_pmcperl(ppl, oldcx);
  }
}

void
PMC_DetachPerlProxies(PMCPerl *ppl, JSContext *cx)
{
  if (ppl && cx) {
    JSContext *oldcx;
    oldcx = pmc_enter_pmcperl(ppl, cx);
    pmc_detach_perl_proxies(ppl, cx);
    pmc_leave_pmcperl(ppl, oldcx);
  }
}

void
PMC_DestroyPerl(PMCPerl *ppl, JSContext *cx)
{
  PerlInterpreter *my_perl;
  DBG_API(fprintf(stderr, "PMC_DestroyPerl %p\n", ppl));
  if (!ppl) {
    return;
  }
  my_perl = ppl->interp;
  if (my_perl && ppl->loaded_modules) {
    hv_undef(ppl->loaded_modules);
  }
  PMC_DetachProxies(ppl, cx);
  if (ppl->interp) {
    PerlInterpreter *my_perl = ppl->interp;
    /* ppl->interp is nonnull if ppl->pjg is nonnull */
    if (ppl->pjg) {
      JSContext *oldcx;
      oldcx = pmc_enter_pmcperl(ppl, cx);
      pmc_jsglobal_destroy(my_perl, ppl->pjg);
      pmc_leave_pmcperl(ppl, oldcx);
    }
    perl_destruct(aTHX);
    perl_free(aTHX);
  }
  if (ppl->pjs_list) {
    pmc_list_destroy(ppl->pjs_list, "PMC_DestroyPerl");
  }
  DBG_LIST(fprintf(stderr, "PMC_DestroyPerl %p : pjslist cleared\n", ppl));
  if (ppl->pmo_list) {
    pmc_list_destroy(ppl->pmo_list, "PMC_DestroyPerl");
  }
  if (ppl->pmo_finalized_list) {
    pmc_list_destroy(ppl->pmo_finalized_list, "PMC_DestroyPerl");
  }
  if (ppl->pmo_lock) {
    PR_DestroyLock(ppl->pmo_lock);
  }
  if (ppl->monitor) {
    PR_DestroyMonitor(ppl->monitor);
  }
  DBG_LIST(fprintf(stderr, "PMC_DestroyPerl %p : pmolist cleared\n", ppl));
  pmc_free(ppl, "PMC_DestroyPerl");
}

void
PMC_SetContextThread(PMCPerl *ppl)
{
  assert(ppl);
  assert(ppl->interp);
  DBG_API(fprintf(stderr, "PMC_SetContextThread %p\n", ppl));
  DBG_THREAD(fprintf(stderr, "PMC_SetContextThread %p\n", ppl));
  PERL_SET_CONTEXT(ppl->interp);
}

void
PMC_ClearContextThread(PMCPerl *ppl)
{
}

static int
pmc_init_pmcglobal_on_perl(PMCPerl *ppl, JSContext *cx, JSObject *obj)
{
  if (ppl->interp && !ppl->pjg) {
    PerlInterpreter *my_perl;
    SV *sv, *pjgsv, *glsv;
    my_perl = ppl->interp;
    DBG_JSGL(fprintf(stderr, "jsgl: cx=[%p] obj=[%p]\n", cx, obj));
    ppl->pjg = pmc_jsglobal_new(my_perl, ppl, cx, obj);
    if (!ppl->pjg) {
      return -1;
    }
    pjgsv = pmc_jsglobal_getsv(ppl->pjg);
    sv = get_sv("JavaScript::PMCGlobal", TRUE);
    sv_setsv(sv, pjgsv);
    SvREADONLY_on(sv);
    glsv = pmc_jsval_to_sv(aTHX_ ppl, cx, OBJECT_TO_JSVAL(obj), 0);
    DBG_JSGL(fprintf(stderr, "glsv=%p\n", glsv));
    sv = get_sv("JavaScript::Global", TRUE);
    sv_setsv(sv, glsv);
    SvREADONLY_on(sv);
    SvREFCNT_dec(glsv);
    return 0;
  } else {
    return -1;
  }
}

JSObject *
PMC_InitClasses(PMCPerl *ppl, JSContext *cx, JSObject *obj)
{
  JSObject *pom = NULL, *pproto = NULL;
  JSContext *oldcx;
  oldcx = pmc_enter_pmcperl(ppl, cx);

  pom = pmc_init_pmc_exception_class(cx, obj);
  if (!pom) {
    goto out;
  }
  pproto = pmc_init_perl_class(ppl->interp, ppl, cx, obj);
  if (pproto) {
    if (pmc_init_pmcglobal_on_perl(ppl, cx, obj) < 0) {
      pproto = NULL;
      goto out;
    }
  }

 out:

  pmc_leave_pmcperl(ppl, oldcx);
  return pproto;
}

int
PMC_SetArgv(PMCPerl *ppl, JSContext *cx, JSObject *obj, int argc,
  char **argv)
{
  PerlInterpreter *my_perl;
  AV *av;
  int i;
  int ret = -1;
  JSObject *arr = NULL;
  JSContext *oldcx = NULL;

  my_perl = pmc_get_interpreter(ppl);
  oldcx = pmc_enter_pmcperl(ppl, cx);

  ENTER;
  SAVETMPS;

  av = get_av("ARGV", 0);
  av_clear(av);
  for (i = 0; i < argc; ++i) {
    SV *sv;
    sv = newSVpv(argv[i], 0);
    if (!av_store(av, i, sv)) {
      SvREFCNT_dec(sv);
      goto out;
    }
  }

  arr = JS_NewArrayObject(cx, 0, NULL);
  if (!arr) {
    goto out;
  }
  if (!JS_DefineProperty(cx, obj, "arguments", OBJECT_TO_JSVAL(arr),
    NULL, NULL, 0)) {
    goto out;
  }
  for (i = 0; i < argc; ++i) {
    JSString *str = JS_NewStringCopyZ(cx, argv[i]);
    if (!str) {
      goto out;
    }
    if (!JS_DefineElement(cx, arr, i, STRING_TO_JSVAL(str), NULL, NULL,
      JSPROP_ENUMERATE)) {
      goto out;
    }
  }
  /* success */
  ret = 0;

 out: 

  FREETMPS;
  LEAVE;

  pmc_leave_pmcperl(ppl, oldcx);
  return ret;
}


PerlInterpreter *
pmc_get_interpreter(PMCPerl *ppl)
{
  return ppl ? ppl->interp : NULL;
}

pmc_jsglobal *
pmc_get_pmc_jsglobal(PMCPerl *ppl)
{
  return ppl ? ppl->pjg : NULL;
}

JSContext *
pmc_get_current_jsctx(PMCPerl *ppl)
{
  return ppl ? ppl->current_jsctx : NULL;
}

JSContext *
pmc_enter_pmcperl(PMCPerl *ppl, JSContext *newjsctx)
{
  JSContext *r;
  PR_EnterMonitor(ppl->monitor);
  r = ppl->current_jsctx;
  ppl->current_jsctx = newjsctx;
  PERL_SET_CONTEXT(ppl->interp);
  return r;
}

void
pmc_leave_pmcperl(PMCPerl *ppl, JSContext *oldjsctx)
{
  pmc_clean_finalized(ppl, ppl->current_jsctx);
  ppl->current_jsctx = oldjsctx;
  PR_ExitMonitor(ppl->monitor);
}

pmc_listnode *
pmc_register_perl_proxy(PMCPerl *ppl, JSContext *cx, JSObject *obj)
{
  pmc_listnode *nd;
  PR_Lock(ppl->pmo_lock);
  nd = pmc_list_insert_before(ppl->pmo_list, NULL, obj);
  DBG_PPX(fprintf(stderr, "___regppx %p\n", nd));
  if (!nd) {
    pmc_report_js_outofmemory(cx);
  }
  PR_Unlock(ppl->pmo_lock);
  return nd;
}

void
pmc_unregister_perl_proxy_nolock(PMCPerl *ppl, pmc_listnode *nd)
{
  DBG_PPX(fprintf(stderr, "___unregppx %p\n", nd));
  pmc_list_erase(nd);
}

void
pmc_unregister_perl_proxy(PMCPerl *ppl, pmc_listnode *nd)
{
  PR_Lock(ppl->pmo_lock);
  pmc_unregister_perl_proxy_nolock(ppl, nd);
  PR_Unlock(ppl->pmo_lock);
}

void
pmc_detach_perl_proxies(PMCPerl *ppl, JSContext *cx)
{
  pmc_listnode *i;
  PR_Lock(ppl->pmo_lock);
  for (i = pmc_list_first(ppl->pmo_list); i; ) {
    pmc_listnode *nxt;
    nxt = pmc_list_next(ppl->pmo_list, i);
    pmc_plproxy_detach_plpriv_nolock(ppl->interp, ppl, cx,
      (JSObject *)pmc_listnode_get_value(i));
    i = nxt;
  }
  PR_Unlock(ppl->pmo_lock);
}

void
pmc_clean_finalized(PMCPerl *ppl, JSContext *cx)
{
  pmc_listnode *i;
  PR_Lock(ppl->pmo_lock);
  for (i = pmc_list_first(ppl->pmo_finalized_list); i; ) {
    pmc_listnode *nxt;
    pmc_plpriv *pp;
    nxt = pmc_list_next(ppl->pmo_finalized_list, i);
    pp = (pmc_plpriv *)pmc_listnode_get_value(i);
    pmc_plpriv_destroy(pp);
    pmc_unregister_perl_proxy_nolock(ppl, i);
    i = nxt;
  }
  PR_Unlock(ppl->pmo_lock);
}

void
pmc_moveto_finalized_list_noperllock(PMCPerl *ppl, JSContext *cx,
  pmc_listnode *nd, void *pp)
{
  PR_Lock(ppl->pmo_lock);
  DBG_PPX(fprintf(stderr, "___finalized %p\n", nd));
  pmc_listnode_set_value(nd, pp);
  pmc_list_move_last(ppl->pmo_finalized_list, nd); /* reuse nd */
  PR_Unlock(ppl->pmo_lock);
}

pmc_listnode *
pmc_register_js_proxy(PMCPerl *ppl, JSContext *cx, pmc_jsobject *pjo)
{
  pmc_listnode *nd;
  nd = pmc_list_insert_before(ppl->pjs_list, NULL, pjo);
  DBG_PPX(fprintf(stderr, "___regjpx %p\n", nd));
  if (!nd) {
    pmc_report_js_outofmemory(cx);
  }
  return nd;
}

void
pmc_unregister_js_proxy(PMCPerl *ppl, pmc_listnode *nd)
{
  DBG_PPX(fprintf(stderr, "___unregjpx %p\n", nd));
  pmc_list_erase(nd);
}

void
pmc_detach_js_proxies(PMCPerl *ppl, JSContext *cx)
{
  pmc_listnode *i;
  for (i = pmc_list_first(ppl->pjs_list); i; ) {
    pmc_listnode *nxt;
    nxt = pmc_list_next(ppl->pjs_list, i);
    pmc_jsobject_detach_js(ppl->interp,
      (pmc_jsobject *)pmc_listnode_get_value(i), cx);
    i = nxt;
  }
}

void
pmc_set_suppress_conv(PMCPerl *ppl, int suppress)
{
  ppl->suppress_conv = suppress;
}

int
pmc_get_suppress_conv(PMCPerl *ppl)
{
  return ppl->suppress_conv;
}

int
pmc_remember_loaded(PMCPerl *ppl, const char *name)
{
  PerlInterpreter *my_perl;
  SV **elemp;
  int r = 0;
  my_perl = pmc_get_interpreter(ppl);
  ENTER;
  SAVETMPS;
  if (!ppl->loaded_modules) {
    return 0;
  }
  elemp = hv_fetch(ppl->loaded_modules, name, strlen(name), 1);
  if (!elemp || !(*elemp)) {
    return 0;
  }
  if (SvIOK(*elemp) && SvIV(*elemp)) {
    r = 1;
  } else {
    SV *nsv;
    nsv = newSViv(1);
    SvSetSV(*elemp, nsv);
  }
  DBG_MOD(fprintf(stderr, "loaded: %s %d\n", name, r));
  FREETMPS;
  LEAVE;
  return r;
}

