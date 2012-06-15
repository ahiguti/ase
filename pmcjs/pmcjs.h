
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef PMCJS_HPP
#define PMCJS_HPP

#include <jsapi.h> /* SpiderMonkey */

#ifdef _MSC_VER
#ifdef PMCJS_EXPORTS
#define PMCJS_EXT __declspec(dllexport)
#else
#define PMCJS_EXT __declspec(dllimport)
#endif
#else
#define PMCJS_EXT
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PMCPerl_s PMCPerl;

typedef int PMC_PerlAvailable_f();
typedef void PMC_Init_f(int argc, char **argv, char **env);
typedef void PMC_Terminate_f();
typedef PMCPerl *PMC_NewPerl_f();
typedef void PMC_DestroyPerl_f(PMCPerl *ppl, JSContext *cx);
typedef JSObject *PMC_InitClasses_f(PMCPerl *ppl, JSContext *cx,
  JSObject *obj);
typedef int PMC_SetArgv_f(PMCPerl *ppl, JSContext *cx, JSObject *obj,
  int argc, char **argv);
typedef void PMC_SetCallback_f(PMCPerl *(*)(JSContext *)); /* Obsolete */
typedef void PMC_SetContextThread_f(PMCPerl *ppl);
typedef void PMC_ClearContextThread_f(PMCPerl *ppl);
typedef JSBool PMC_Eval_f(PMCPerl *ppl, JSContext *cx, const char *str);
typedef JSBool PMC_EvalFile_f(PMCPerl *ppl, JSContext *cx,
  const char *str, const char *filename);
typedef JSBool PMC_Require_f(PMCPerl *ppl, JSContext *cx,
  const char *filename);
typedef void PMC_DetachProxies_f(PMCPerl *ppl, JSContext *cx);
typedef void PMC_DetachJSProxies_f(PMCPerl *ppl, JSContext *cx);
typedef void PMC_DetachPerlProxies_f(PMCPerl *ppl, JSContext *cx);

PMCJS_EXT PMC_PerlAvailable_f PMC_PerlAvailable;
PMCJS_EXT PMC_Init_f PMC_Init;
PMCJS_EXT PMC_Terminate_f PMC_Terminate;
PMCJS_EXT PMC_NewPerl_f PMC_NewPerl;
PMCJS_EXT PMC_DestroyPerl_f PMC_DestroyPerl;
PMCJS_EXT PMC_InitClasses_f PMC_InitClasses;
PMCJS_EXT PMC_SetArgv_f PMC_SetArgv;
PMCJS_EXT PMC_SetCallback_f PMC_SetCallBack;
PMCJS_EXT PMC_SetContextThread_f PMC_SetContextThread;
PMCJS_EXT PMC_ClearContextThread_f PMC_ClearContextThread;
PMCJS_EXT PMC_Eval_f PMC_Eval;
PMCJS_EXT PMC_EvalFile_f PMC_EvalFile;
PMCJS_EXT PMC_Require_f PMC_Require;
PMCJS_EXT PMC_DetachProxies_f PMC_DetachProxies;
PMCJS_EXT PMC_DetachJSProxies_f PMC_DetachJSProxies;
PMCJS_EXT PMC_DetachPerlProxies_f PMC_DetachPerlProxies;

typedef struct {
  PMC_PerlAvailable_f *PMC_PerlAvailable;
  PMC_Init_f *PMC_Init;
  PMC_Terminate_f *PMC_Terminate;
  PMC_NewPerl_f *PMC_NewPerl;
  PMC_DestroyPerl_f *PMC_DestroyPerl;
  PMC_InitClasses_f *PMC_InitClasses;
  PMC_SetArgv_f *PMC_SetArgv;
  PMC_SetContextThread_f *PMC_SetContextThread;
  PMC_ClearContextThread_f *PMC_ClearContextThread;
  PMC_Eval_f *PMC_Eval;
  PMC_EvalFile_f *PMC_EvalFile;
  PMC_Require_f *PMC_Require;
  PMC_DetachProxies_f *PMC_DetachProxies;
  PMC_DetachJSProxies_f *PMC_DetachJSProxies;
  PMC_DetachPerlProxies_f *PMC_DetachPerlProxies;
} PMC_APIFunctionTable_2;

PMCJS_EXT PMC_APIFunctionTable_2 *PMC_GetAPIFunctionTable_2();

#ifdef __cplusplus
}
#endif

#endif

