
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <assert.h>
#include <string.h>

#include "pjsobjiter.h"
#include "poutil_js.h"
#include "utfutil.h"

struct pmc_jsobjiter_s {
  char **arr;
  size_t arrsize;
  size_t current;
};

/* returns NULL iff malloc failed */
pmc_jsobjiter *
pmc_jsobjiter_new(JSContext *cx, JSObject *obj)
{
  pmc_jsobjiter *iter = NULL;
  JSIdArray *idarr = NULL;
  jsval idv = JSVAL_VOID;
  char **arr = NULL;
  int len = 0, i = 0;
  if (!JS_AddNamedRoot(cx, &idv, "pmc_jsobjiter")) {
    return NULL;
  }
  iter = (pmc_jsobjiter *)pmc_malloc(cx, sizeof(*iter), "pmc_jsobjiter1");
  if (!iter) {
    goto err;
  }
  memset(iter, 0, sizeof(*iter));
  iter->arr = NULL;
  iter->arrsize = 0;
  iter->current = 0;
  idarr = JS_Enumerate(cx, obj);
  if (!idarr) {
    goto err;
  }
  len = idarr->length;
  arr = (char **)pmc_malloc(cx, sizeof(*arr) * len, "pmc_jsobjiter2");
  if (!arr) {
    goto err;
  }
  for (i = 0; i < len; ++i) {
    arr[i] = NULL;
  }
  for (i = 0; i < len; ++i) {
    jsid id;
    JSString *ids;
    pmc_u16ch_t *u16str;
    char *u8str;
    size_t u16len = 0, u8len = 0;
    id = idarr->vector[i];
    if (!JS_IdToValue(cx, id, &idv)) {
      goto err;
    }
    ids = JS_ValueToString(cx, idv);
    if (!ids) {
      goto err;
    }
    u16str = JS_GetStringChars(ids);
    u16len = JS_GetStringLength(ids);
    u8str = pmc_new_utf8str_from_utf16str(cx, u16str, u16len, &u8len,
      "pmc_jsobjiter3");
    if (!u8str) {
      goto err;
    }
    arr[i] = u8str;
  }
  iter->arr = arr;
  iter->arrsize = (size_t)len;
  JS_RemoveRoot(cx, &idv);
  return iter;

 err:
  if (arr) {
    for (i = 0; i < len; ++i) {
      if (arr[i]) {
	pmc_free(arr[i], "pmc_jsobjiter");
      }
    }
    pmc_free(arr, "pmc_jsobjiter");
  }
  if (idarr) {
    JS_DestroyIdArray(cx, idarr);
  }
  if (iter) {
    pmc_free(iter, "pmc_jsobjiter");
  }
  JS_RemoveRoot(cx, &idv);
  return NULL;
}

void
pmc_jsobjiter_destroy(pmc_jsobjiter *iter)
{
  size_t i;
  assert(iter);
  if (iter->arr) {
    for (i = 0; i < iter->arrsize; ++i) {
      if (iter->arr[i]) {
	pmc_free(iter->arr[i], "pmc_jsobjiter_destroy_arrelem");
      }
    }
    pmc_free(iter->arr, "pmc_jsobjiter_arr");
  }
  memset(iter, 0, sizeof(*iter));
  pmc_free(iter, "pmc_jsobjiter");
}

const char *
pmc_jsobjiter_next(pmc_jsobjiter *iter)
{
  const char *r;
  assert(iter);
  if (iter->current >= iter->arrsize) {
    return NULL;
  }
  r = iter->arr[iter->current];
  iter->current++;
  return r;
}

