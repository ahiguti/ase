
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef UTFUTIL_H
#define UTFUTIL_H

#include "utfconv.h"

#ifdef __cplusplus
extern "C" {
#endif

int pmc_utf8_is_7bit_string(const char *str, size_t len);
int pmc_utf16_is_7bit_string(pmc_u16ch_t *str, size_t len);
/* use pmc_free() to destroy */
char *pmc_new_utf8str_from_utf16str(JSContext *cx, const pmc_u16ch_t *str,
  size_t len, size_t *u8len_r, const char *debugid);
char *pmc_new_utf8str_from_utf16str_nothrow(JSContext *cx,
  const pmc_u16ch_t *str, size_t len, size_t *u8len_r, pmc_uconv_err_t *err_r,
  const char *debugid);
/* use pmc_free() to destroy */
pmc_u16ch_t *pmc_new_utf16str_from_utf8str(JSContext *cx, const char *str,
  size_t len, size_t *u16len_r, const char *debugid);
pmc_u16ch_t *pmc_new_utf16str_from_utf8str_nothrow(JSContext *cx,
  const char *str, size_t len, size_t *u16len_r, pmc_uconv_err_t *err_r,
  const char *debugid);
size_t pmc_utf16_strlen(const pmc_u16ch_t *str);
void pmc_utf16_hexdump(const pmc_u16ch_t *str);

#ifdef __cplusplus
}
#endif

#endif

