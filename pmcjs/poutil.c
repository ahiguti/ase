
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "poutil_js.h"
#include "pmconv.h"
#include "pmcjs_impl.h"
#include "pmcexception.h"
#include "utfutil.h"
#include <jsdbgapi.h>

#define DBG_MEM(x)
#define DBG_FRAME(x)
#define DBG_EXC(x)

void *
pmc_malloc_nocx(size_t len, const char *debugid)
{
  void *p;
  p = malloc(len ? len : 1);
  DBG_MEM(fprintf(stderr, "__m %p %s\n", p, debugid));
  return p;
}

void *
pmc_malloc(JSContext *cx, size_t len, const char *debugid)
{
  void *p;
  p = pmc_malloc_nocx(len, debugid);
  if (!p) {
    pmc_report_js_outofmemory(cx);
  }
  return p;
}

void
pmc_free(void *p, const char *debugid)
{
  if (p) {
    DBG_MEM(fprintf(stderr, "__f %p %s\n", p, debugid));
  }
  free(p);
}

char *
pmc_strdup(JSContext *cx, const char *str, const char *debugid)
{
  size_t len = 0;
  char *p = NULL;
  len = strlen(str);
  p = (char *)pmc_malloc(cx, len + 1, debugid);
  if (!p) {
    return NULL;
  }
  memcpy(p, str, len + 1);
  return p;
}

char *
pmc_strcat(JSContext *cx, const char *str1, const char *str2,
  const char *debugid)
{
  size_t len1 = 0, len2 = 0;
  char *p = NULL;
  len1 = strlen(str1);
  len2 = strlen(str2);
  p = (char *)pmc_malloc(cx, len1 + len2 + 1, debugid);
  if (!p) {
    return NULL;
  }
  memcpy(p, str1, len1);
  memcpy(p + len1, str2, len2 + 1);
  return p;
}

void
pmc_dump_jsval(JSContext *cx, const char *mess, jsval jv)
{
  JSString *str;
  const char *klname = NULL;
  str = JS_ValueToString(cx, jv);
  if (JSVAL_IS_OBJECT(jv)) {
    klname = JS_GetClass(cx, JSVAL_TO_OBJECT(jv))->name;
  }
  fprintf(stderr, "%s: tag=%d kl=%s %s\n", mess, (int)JSVAL_TAG(jv), klname,
    (str ? JS_GetStringBytes(str) : ""));
}

void
pmc_dump_str(const char *mess, const char *str, size_t slen)
{
  char *scp;
  scp = pmc_malloc_nocx(slen + 1, "pmc_dump_str");
  if (scp == NULL) {
    fprintf(stderr, "%s: dump: pmc_malloc failed\n", mess);
  } else {
    memcpy(scp, str, slen);
    scp[slen] = '\0';
    fprintf(stderr, "%s: %s\n", mess, scp);
    pmc_free(scp, "pmc_dump_str");
  }
}

static void
pmc_drop_ctrl_chars(pmc_u16ch_t *u16str, size_t u16len)
{
  size_t i;
  for (i = 0; i < u16len; ++i) {
    pmc_u16ch_t c = u16str[i];
    if (c < 0x80 && iscntrl(c)) {
      u16str[i] = ' ';
    }
  }
}

static JSBool
pmc_set_str_property(JSContext *cx, JSObject *obj, const char *prop,
  const char *value)
{
  JSString *vstr;
  pmc_u16ch_t *u16str = NULL;
  size_t u16len = 0;
  size_t u8len = 0;
  JSBool suc = JS_FALSE;
  jsval jv = JSVAL_VOID;

  u8len = strlen(value);
  u16str = pmc_new_utf16str_from_utf8str(cx, value, u8len, &u16len,
    "pmc_set_str_property");
  if (!u16str) {
    return JS_FALSE;
  }
  pmc_drop_ctrl_chars(u16str, u16len);
  vstr = JS_NewUCStringCopyN(cx, u16str, u16len);
  pmc_free(u16str, "pmc_set_str_property");
  if (!vstr) {
    return JS_FALSE;
  }
  jv = STRING_TO_JSVAL(vstr);
  suc = JS_SetProperty(cx, obj, prop, &jv);
  return suc;
}

static JSBool
pmc_throw_js_exception_internal(JSContext *cx, const char *etype,
  const char *err, const char *func, const char *description)
{
  JSObject *obj = NULL;
  JSObject *obj2 = NULL;
  JSObject *arr = NULL;
  jsval jv = JSVAL_VOID;
  jsval arrv = JSVAL_VOID;
  JSStackFrame *fp = NULL;
  int idx = 0;

  DBG_EXC(fprintf(stderr, "EXC_THROW\n"));
  if (JS_IsExceptionPending(cx)) {
    return JS_FALSE;
  }

  // obj = JS_NewObject(cx, NULL, NULL, NULL); // FIXME: remove
  obj = JS_NewObject(cx, &pmc_exception_class, NULL, NULL);
  if (!obj) {
    goto err;
  }
  jv = OBJECT_TO_JSVAL(obj);
  DBG_EXC(fprintf(stderr, "exc obj=%p jv=%ld\n", obj, jv));
  JS_SetPendingException(cx, jv);
  if (!pmc_set_str_property(cx, obj, "etype", etype) ||
    !pmc_set_str_property(cx, obj, "error", err) ||
    !pmc_set_str_property(cx, obj, "func", func) ||
    !pmc_set_str_property(cx, obj, "description", description)) {
    goto err;
  }

  DBG_EXC(fprintf(stderr, "exc arr\n"));
  arr = JS_NewArrayObject(cx, 0, NULL);
  if (!arr) {
    goto err;
  }
  DBG_EXC(fprintf(stderr, "exc arr done\n"));
  arrv = OBJECT_TO_JSVAL(arr);
  if (!JS_SetProperty(cx, obj, "at", &arrv)) {
    goto err;
  }
  DBG_EXC(fprintf(stderr, "exc arr setprop done\n"));

  DBG_FRAME(fprintf(stderr, "FRAME BACKTRACE\n"));
  while ((JS_FrameIterator(cx, &fp)) != NULL) {
    JSScript *scr = NULL;
    const char *scrname = NULL;
    JSFunction *func = NULL;
    const char *funcname = NULL;
    uintN lineno = 0;
    jsbytecode *pc = NULL;
    jsval jv;
    JSObject *e;
    scr = JS_GetFrameScript(cx, fp);
    pc = JS_GetFramePC(cx, fp);
    /* scr is null iff it's a native frame */
    if (scr && pc) {
      scrname = JS_GetScriptFilename(cx, scr);
      lineno = JS_PCToLineNumber(cx, scr, pc);
      func = JS_GetFrameFunction(cx, fp);
      funcname = func ? JS_GetFunctionName(func) : NULL;
      DBG_FRAME(fprintf(stderr, "file=%s line=%u\n", scrname, lineno));
      e = JS_NewObject(cx, NULL, NULL, NULL);
      if (!e) {
	goto err;
      }
      jv = OBJECT_TO_JSVAL(e);
      if (!JS_SetElement(cx, arr, idx++, &jv)) {
	goto err;
      }
      if (!pmc_set_str_property(cx, e, "func", funcname ? funcname : "")) {
	goto err;
      }
      if (!pmc_set_str_property(cx, e, "file", scrname)) {
	goto err;
      }
      jv = INT_TO_JSVAL((int)lineno);
      if (!JS_SetProperty(cx, e, "line", &jv)) {
	goto err;
      }
    }
  }

  /* create a wrapper. this is just a hack to prevent the thrown exception
    leaving from the root set when it's caught by a js catch clause. see
    'cx->throwing = JS_FALSE' after SCRIPT_FIND_CATCH_START(...) in
    jsinterp.c(js-1.5) and try TOO_MUCH_GC to check this behavior. */
  obj2 = JS_NewObject(cx, &pmc_exception_class, obj, NULL);
  if (!obj2) {
    goto err;
  }
  JS_SetPendingException(cx, OBJECT_TO_JSVAL(obj2));

  /* always returns JS_FALSE */
  DBG_EXC(fprintf(stderr, "EXC_THROW done\n")); // FIXME
  return JS_FALSE;

 err:

  /* possibly oom */
  DBG_EXC(fprintf(stderr, "EXC_THROW err\n"));
  JS_ClearPendingException(cx);
  return JS_FALSE;
}
  
JSBool
pmc_throw_js_logic_error(JSContext *cx, const char *err, const char *func,
  const char *description)
{
  return pmc_throw_js_exception_internal(cx, "LogicError", err, func,
    description);
}

JSBool
pmc_throw_js_runtime_exception(JSContext *cx, const char *err,
  const char *func, const char *description)
{
  return pmc_throw_js_exception_internal(cx, "RuntimeException", err, func,
    description);
}

JSBool
pmc_report_js_outofmemory(JSContext *cx)
{
  JS_ReportOutOfMemory(cx);
  return JS_FALSE;
}

JSBool
pmc_throw_js_nopmc(JSContext *cx, const char *func)
{
  return pmc_throw_js_logic_error(cx, "NoPerl", func,
    "not attached to a perl object");
}

