
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include "utfconv.h"
#include "poutil.h"
#include "poutil_js.h"
#include "utfutil.h"

char * /* use pmc_free() to destroy. returns NULL iff outofmemory. */
pmc_new_utf8str_from_utf16str_nothrow(JSContext *cx, const pmc_u16ch_t *str,
  size_t len, size_t *u8len_r, pmc_uconv_err_t *err_r, const char *debugid)
{
  /* TODO: not optimal */
  const pmc_u16ch_t *u16s = NULL;
  const pmc_u16ch_t *u16pos = NULL;
  pmc_u8ch_t *u8s = NULL;
  pmc_u8ch_t *u8pos = NULL;
  size_t u8len = 0;
  unsigned long pos = 0;
  pmc_uconv_err_t err = pmc_uconv_err_success;

  (*u8len_r) = 0;
  (*err_r) = pmc_uconv_err_success;
  u8len = len * 3;
  u16s = str;
  u8s = (pmc_u8ch_t *)pmc_malloc(cx,
    (u8len + 1) * sizeof(*u8s /* must be 1 */),
    debugid); /* plus one for nul term */
  if (!u8s) {
    return NULL;
  }
  err = pmc_uconv_16to8(u16s, u16s + len, u8s, u8s + u8len, &u16pos,
    &u8pos);
  (*u8pos) = 0; /* nul term */
  pos = (u8pos - u8s);
  (*u8len_r) = pos;
  (*err_r) = err;
  return (char *)u8s;
}

char * /* use pmc_free() to destroy. returns NULL on error. */
pmc_new_utf8str_from_utf16str(JSContext *cx, const pmc_u16ch_t *str,
  size_t len, size_t *u8len_r, const char *debugid)
{
  char *s;
  pmc_uconv_err_t err = pmc_uconv_err_success;
  s = pmc_new_utf8str_from_utf16str_nothrow(cx, str, len, u8len_r, &err,
    debugid);
  if (s && err != pmc_uconv_err_success) {
    pmc_free(s, debugid);
    s = NULL;
    pmc_throw_js_runtime_exception(cx, "ConvertionFailed",
      "pmc_new_utf8str_from_utf16str", "utf16 to utf8 failed");
  }
  return s;
}

pmc_u16ch_t * /* use pmc_free() to destroy. returns NULL iff outofmemory. */
pmc_new_utf16str_from_utf8str_nothrow(JSContext *cx, const char *str,
  size_t len, size_t *u16len_r, pmc_uconv_err_t *err_r, const char *debugid)
{
  /* TODO: not optimal */
  const pmc_u8ch_t *u8s = NULL;
  const pmc_u8ch_t *u8pos = NULL;
  pmc_u16ch_t *u16s = NULL;
  pmc_u16ch_t *u16pos = NULL;
  size_t u16len = 0;
  unsigned long pos = 0;
  pmc_uconv_err_t err = pmc_uconv_err_success;

  (*u16len_r) = 0;
  (*err_r) = pmc_uconv_err_success;
  u16len = len;
  u8s = (const pmc_u8ch_t *)str;
  u16s = (pmc_u16ch_t *)pmc_malloc(cx, (u16len + 1) * sizeof(*u16s), debugid);
    /* plus one for nul term */
  if (!u16s) {
    return NULL;
  }
  err = pmc_uconv_8to16(u8s, u8s + len, u16s, u16s + u16len, &u8pos,
    &u16pos);
  (*u16pos) = 0; /* nul term */
  pos = (u16pos - u16s);
  (*u16len_r) = pos;
  (*err_r) = err;
  return u16s;
}

pmc_u16ch_t * /* use pmc_free() to destroy. returns NULL on error. */
pmc_new_utf16str_from_utf8str(JSContext *cx, const char *str,
  size_t len, size_t *u16len_r, const char *debugid)
{
  pmc_u16ch_t *s;
  pmc_uconv_err_t err = pmc_uconv_err_success;
  s = pmc_new_utf16str_from_utf8str_nothrow(cx, str, len, u16len_r, &err,
    debugid);
  if (s && err != pmc_uconv_err_success) {
    pmc_free(s, debugid);
    s = NULL;
    pmc_throw_js_runtime_exception(cx, "ConvertionFailed",
      "pmc_new_utf16str_from_utf8str", "utf8 to utf16 failed");
  }
  return s;
}

int
pmc_utf16_is_7bit_string(pmc_u16ch_t *ustr, size_t len)
{
  size_t i = 0;
  for (i = 0; i < len; ++i) {
    if ((ustr[i] & 0xff80) != 0) {
      return 0;
    }
  }
  return 1;
}

int
pmc_utf8_is_7bit_string(const char *str, size_t len)
{
  size_t i = 0;
  const unsigned char *ustr = (const unsigned char *)str;
  for (i = 0; i < len; ++i) {
    if ((ustr[i] & 0x80) != 0) {
      return 0;
    }
  }
  return 1;
}

size_t
pmc_utf16_strlen(const pmc_u16ch_t *str)
{
  size_t i = 0;
  if (!str) {
    return 0;
  }
  for (i = 0; str[i] != 0; ++i) { }
  return i;
}

void
pmc_utf16_hexdump(const pmc_u16ch_t *str)
{
  size_t len = 0, i = 0;
  len = pmc_utf16_strlen(str);
  for (i = 0; i < len; ++i) {
    printf(" %04x", str[i]);
  }
  printf("\n");
}


