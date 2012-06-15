
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <stdlib.h>
#include <string.h>
#include "pmcjs.h"
#include "utfconv.h"
#include "utfutil.h"
#include "poutil.h"
#include "poutil_js.h"

#include <jscntxt.h> // tracefp

#define DBG_TRACE(x)

static void
die_unless(int c, const char *mess)
{
  if (!c) {
    fprintf(stderr, "Fatal: %s\n", mess);
    exit(1);
  }
}

static void
set_pmcperl(JSContext *cx, PMCPerl *pl)
{
  JS_SetContextPrivate(cx, pl);
}

static PMCPerl *
get_pmcperl(JSContext *cx)
{
  return (PMCPerl *)JS_GetContextPrivate(cx);
}

static pmc_u16ch_t * /* use pmc_free() to destroy */
new_utf16str_from_file(JSContext *cx, const char *filename, size_t *u16len_r,
  pmc_uconv_err_t *err_r)
{
  FILE *fp = NULL;
  long filelen = 0;
  void *buf = NULL;
  int dump = 0;
  pmc_u16ch_t *u16str = NULL;

  (*u16len_r) = 0;
  (*err_r) = pmc_uconv_err_success;
  fp = fopen(filename, "r");
  if (!fp) {
    return NULL;
  }
  fseek(fp, 0, SEEK_END); /* file must be seekable */
  filelen = ftell(fp);
  if (filelen == 0) {
    goto end;
  }
  rewind(fp);
  buf = pmc_malloc(cx, filelen, "new_utf16str_from_file");
  if (!buf) {
    goto end;
  }
  if (fread(buf, filelen, 1, fp) != 1) {
    goto end;
  }
  if (dump) {
    long i;
    for (i = 0; i < filelen; ++i) {
      printf("%02x ", ((unsigned char *)buf)[i]);
    }
    printf("\n");
  }
  u16str = pmc_new_utf16str_from_utf8str_nothrow(cx, buf, filelen,
    u16len_r, err_r, "new_utf16str_from_file");
  pmc_free(buf, "new_utf16str_from_file");
  return u16str;

 end:

  pmc_free(buf, "new_utf16str_from_file");
  if (fp) {
    fclose(fp);
  }
  return NULL;
}

static JSBool
exec_script_js(JSContext *cx, JSObject *obj, JSPrincipals *pr,
  const char *filename)
{
  JSScript *scr = NULL;
  pmc_u16ch_t *wbuf = NULL;
  size_t wblen = 0;
  size_t skip = 0;
  int dump = 0;
  pmc_uconv_err_t err = pmc_uconv_err_success;
  jsval result = JSVAL_VOID;
  JSBool suc = JS_FALSE;
  int linenumbase = 1;

  wbuf = new_utf16str_from_file(cx, filename, &wblen, &err);
  if (!wbuf) {
    JS_ReportError(cx, "failed to load %s", filename);
    JS_ClearPendingException(cx);
    return JS_FALSE;
  }
  if (err) {
    JS_ReportError(cx, "wrong utf8 sequence: filename=%s offset=%lu err=%d\n",
      filename, (unsigned long)wblen, (int)err);
    goto out;
  }
  if (dump) {
    size_t i;
    for (i = 0; i < wblen; ++i) {
      printf("%04x ", wbuf[i]);
    }
    printf("\n");
  }
  if (skip < wblen && wbuf[0] == '#') {
    while (skip < wblen && wbuf[skip] != '\n') {
      ++skip;
    }
    ++skip;
    linenumbase = 2;
  }
  if (pr) {
    scr = JS_CompileUCScriptForPrincipals(cx, obj, pr, wbuf + skip,
      wblen - skip, filename, linenumbase);
  } else {
    scr = JS_CompileUCScript(cx, obj, wbuf + skip, wblen - skip, filename,
      linenumbase);
  }
  if (!scr) {
    goto out;
  }
  if (!JS_ExecuteScript(cx, obj, scr, &result)) {
    goto out;
  }
  suc = JS_TRUE;

 out:

  if (scr) {
    JS_DestroyScript(cx, scr);
  }
  if (wbuf) {
    pmc_free(wbuf, "exec_script");
  }

  return suc;
}

static JSBool
exec_script_pl(JSContext *cx, JSObject *obj, const char *filename)
{
  char *buf = NULL;
  FILE *fp = NULL;
  long filelen = 0;
  JSBool suc = JS_FALSE;
  PMCPerl *ppl = get_pmcperl(cx);
  if (!ppl) {
    JS_ReportError(cx, "exec_script_pl: failed to get pmcperl");
    return JS_FALSE;
  } 
  fp = fopen(filename, "r");
  if (!fp) {
    JS_ReportError(cx, "exec_script_pl: failed to open %s", filename);
    return JS_FALSE;
  }
  fseek(fp, 0, SEEK_END); /* file must be seekable */
  filelen = ftell(fp);
  if (filelen == 0) {
    goto out;
  }
  rewind(fp);
  buf = pmc_malloc(cx, filelen, "exec_script_pl");
  if (!buf) {
    JS_ReportOutOfMemory(cx);
    return JS_FALSE;
  }
  if (fread(buf, filelen, 1, fp) != 1) {
    goto out;
  }
  buf[filelen] = '\0';
  if (PMC_EvalFile(ppl, cx, &buf[0], filename) == 0) {
    JS_ReportError(cx, "exec_script_pl: failed to execute %s", filename);
    suc = JS_FALSE;
    goto out;
  }
  suc = JS_TRUE;

 out:

  if (fp) {
    fclose(fp);
  }
  pmc_free(buf, "exec_script_pl");
  return suc;
}

static JSBool
exec_script(JSContext *cx, JSObject *obj, JSPrincipals *pr,
  const char *filename)
{
  int len;
  len = strlen(filename);
  if (len > 3 && strcmp(filename + len - 3, ".pl") == 0) {
    return exec_script_pl(cx, obj, filename);
  }
  return exec_script_js(cx, obj, pr, filename);
}

static void
err_report(JSContext *cx, const char *message, JSErrorReport *report)
{
  if (report->filename) {
    fprintf(stderr, "error: file %s at line %u: %s\n", report->filename,
      report->lineno, message);
  } else {
    fprintf(stderr, "error: %s\n", message);
  }
}

static JSBool
func_print_binary(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
  jsval *rval)
{
  uintN i;
  JSString *str;
  for (i = 0; i < argc; ++i) {
    str = JS_ValueToString(cx, argv[i]);
    if (!str) {
      continue;
    }
    fwrite(JS_GetStringBytes(str), JS_GetStringLength(str), 1, stdout);
  }
  return JS_TRUE;
}

static JSBool
func_gc(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
  jsval *rval)
{
  int verbose = 0;
  if (argc > 0) {
    verbose = 1;
  }
  JS_GC(cx);
  if (verbose) {
    fprintf(stderr, "gc\n");
  }
  (*rval) = JSVAL_VOID;
  return JS_TRUE;
}

static JSBool
func_dump(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
  jsval *rval)
{
  uintN i;
  for (i = 0; i < argc; ++i) {
    const pmc_u16ch_t *u16str = NULL;
    size_t u16len = 0;
    size_t j = 0;
    JSString *str;
    str = JS_ValueToString(cx, argv[i]);
    if (!str) {
      continue;
    }
    u16str = JS_GetStringChars(str);
    u16len = JS_GetStringLength(str);
    for (j = 0; j < u16len; ++j) {
      if (j != 0) {
	printf(" ");
      }
      printf("%02x", (unsigned int)u16str[j]);
    }
  }
  return JS_TRUE;
}

static JSBool
func_print_nonl(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
  jsval *rval)
{
  uintN i;
  for (i = 0; i < argc; ++i) {
    JSString *str = NULL;
    const pmc_u16ch_t *u16str = NULL;
    size_t u16len = 0;
    char *u8str = NULL;
    size_t u8len = 0;
    pmc_uconv_err_t err = pmc_uconv_err_success;
    str = JS_ValueToString(cx, argv[i]);
    if (!str) {
      continue;
    }
    u16str = JS_GetStringChars(str);
    u16len = JS_GetStringLength(str);
    u8str = pmc_new_utf8str_from_utf16str_nothrow(cx, u16str, u16len, &u8len,
      &err, "func_print_nonl");
    if (u8str == NULL) {
      return JS_FALSE;
    }
    fwrite(u8str, u8len, 1, stdout);
    pmc_free(u8str, "func_print_nonl");
  }
  return JS_TRUE;
}

static JSBool
func_print(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
  jsval *rval)
{
  func_print_nonl(cx, obj, argc, argv, rval);
  fwrite("\n", 1, 1, stdout);
  return JS_TRUE;
}

static JSBool
func_print_address(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
  jsval *rval)
{
  if (argc > 0) {
    printf("%p\n", JSVAL_TO_OBJECT(argv[0]));
  }
  return JS_TRUE;
}

#if 0
struct rooted {

  rooted(JSContext *cx) : ctx(cx), value(JSVAL_VOID) {
    if (!JS_AddRoot(ctx, &value)) {
      throw std::bad_alloc();
    }
  }

  ~rooted() {
    JS_RemoveRoot(ctx, &value);
  }

 private:
 
  JSContext *ctx;

 public:

  jsval value;

 private:

  rooted(const rooted& x);
  rooted& operator =(const rooted& x);

};
#endif

static JSBool
use_file(JSContext *cx, JSObject *obj, JSPrincipals *pr, const char *fname)
{
  char *pname = NULL;
  JSBool ret = JS_FALSE;
  jsval value = JSVAL_VOID;
  jsval pv = JSVAL_VOID;
  jsval *rooted = NULL;
  if (strcmp(fname, "Perl") == 0) {
    ret = JS_TRUE;
    goto out;
  }
  pname = malloc(strlen(fname) + 6);
  if (!pname) {
    JS_ReportOutOfMemory(cx);
    ret = JS_FALSE;
    goto out;
  }
  memcpy(pname, "jspl_", 5);
  memcpy(pname + 5, fname, strlen(fname) + 1);
  if (!JS_AddRoot(cx, &value)) {
    ret = JS_FALSE;
    goto out;
  }
  rooted = &value;
  if (!JS_GetProperty(cx, obj, "__pmc_use__", &value)) {
    JS_ReportError(cx, "internal error: pmc_use not found");
    ret = JS_FALSE;
    goto out;
  }
  if (!JSVAL_IS_OBJECT(value)) {
    JS_ReportError(cx, "internal error: pmc_use is malformed");
    ret = JS_FALSE;
    goto out;
  }
  if (JS_GetProperty(cx, obj, pname, &pv) && JSVAL_IS_INT(pv)) {
    /* already loaded */
    ret = JS_TRUE;
    goto out;
  }
  pv = INT_TO_JSVAL(1);
  if (!JS_SetProperty(cx, obj, pname, &pv)) {
    JS_ReportError(cx, "internal error: pmc_use: failed to set %s",
      pname);
    ret = JS_FALSE;
    goto out;
  }
  ret = exec_script(cx, obj, pr, fname);

 out:

  free(pname);
  if (rooted) {
    JS_RemoveRoot(cx, rooted);
  }
  return ret;
}

static JSBool
func_use(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
  jsval *rval)
{
  JSString *str = NULL;
  (*rval) = JSVAL_VOID;
  if (argc < 1) {
    return JS_TRUE;
  }
  str = JS_ValueToString(cx, argv[0]);
  if (!str) {
    JS_ReportError(cx, "internal error: func_use: cant convert to string");
    return JS_FALSE;
  }
  // FIXME: principal
  return use_file(cx, obj, NULL, JS_GetStringBytes(str));
}

static void
prepare_global(JSContext *cx, JSObject *obj)
{
  JSObject *xr = NULL;

  die_unless(JS_DefineFunction(cx, obj, "gc", func_gc,
    0, 0) != NULL, "DefineFunction");
  die_unless(JS_DefineFunction(cx, obj, "print_binary", func_print_binary,
    1, 0) != NULL, "DefineFunction");
  die_unless(JS_DefineFunction(cx, obj, "dump", func_dump,
    1, 0) != NULL, "DefineFunction");
  die_unless(JS_DefineFunction(cx, obj, "print_nonl", func_print_nonl, 1, 0)
    != NULL, "DefineFunction");
  die_unless(JS_DefineFunction(cx, obj, "print", func_print, 1, 0) != NULL,
    "DefineFunction");
  die_unless(JS_DefineFunction(cx, obj, "print_address", func_print_address,
    1, 0) != NULL,
    "DefineFunction");
  die_unless(JS_DefineFunction(cx, obj, "use", func_use, 1, 0) != NULL,
    "DefineFunction");

  xr = JS_NewObject(cx, NULL, NULL, NULL);
  die_unless(xr != NULL, "NewObject");
  die_unless(JS_DefineProperty(cx, obj, "__pmc_use__", OBJECT_TO_JSVAL(xr), 
    NULL, NULL, JSPROP_READONLY | JSPROP_PERMANENT), "DefineProperty");

}

#undef TEST_EVAL
#ifdef TEST_EVAL
static void eval_testloop(PMCPerl *ppl)
{
  for (int i = 0; i < 10000000; ++i) {
    PMC_Eval(ppl, " 'abc'; ");
  }
}
#endif

static int
jspl_main(int argc, char **argv)
{
  JSRuntime *rt = NULL;
  JSContext *cx;
  JSObject *glo;
  PMCPerl *pl;
  {
    die_unless((rt = JS_NewRuntime(1* 1024L * 1024L)) != NULL, "NewRuntime");
    die_unless((cx = JS_NewContext(rt, 256L * 1024L)) != NULL, "NewContext");
    JS_SetErrorReporter(cx, err_report);
    DBG_TRACE(cx->tracefp = stderr);
    die_unless(JS_AddNamedRoot(cx, &glo, "jspl_main"), "AddNamedRoot");
    die_unless((glo = JS_NewObject(cx, NULL, NULL, NULL)) != NULL,
      "NewObject");
    die_unless(JS_InitStandardClasses(cx, glo), "InitClasses");
#if 1
    die_unless((pl = PMC_NewPerl()) != NULL, "NewPerl");
    set_pmcperl(cx, pl);
#endif
    die_unless((PMC_InitClasses(pl, cx, glo) != NULL), "InitPMClasses");
    prepare_global(cx, glo);
#if 1
    die_unless((PMC_SetArgv(pl, cx, glo, argc - 2, argv + 2) == 0),
      "SetArgv");
#endif
    if (!use_file(cx, glo, NULL, argv[1])) {
      /* if argv[1] is a perl file, an exception is thrown on error. */
      #if 0
      jsval ex = JSVAL_VOID;
      JS_GetPendingException(cx, &ex);
      dump_jsval(cx, ex);
      #endif
      JS_ReportPendingException(cx);
    }
    #ifdef TEST_EVAL
    eval_testloop(pl);
    #endif
    JS_RemoveRoot(cx, &glo);
#if 1
    PMC_DestroyPerl(pl, cx);
#endif
    set_pmcperl(cx, NULL);
    JS_DestroyContext(cx);
    JS_DestroyRuntime(rt);
  }
  return 0;
}

int main(int argc, char **argv)
{
  char *arr[] = { NULL };
  int r;
  if (argc < 2) {
    printf("usage: %s FILENAME.js [...] \n", argv[0]);
    printf("       %s FILENAME.pl [...] \n", argv[0]);
    return 1;
  }
  PMC_Init(0, arr, NULL);
  r = jspl_main(argc, argv);
  PMC_Terminate();
  JS_ShutDown();
  return r;
}


