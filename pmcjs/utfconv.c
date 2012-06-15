
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifdef GWS_SOURCE
#include "aioport/aio_utfconv.hpp"
namespace aioport {
#else
#include "utfconv.h"
#endif

#if defined(__GNUC__) || defined(GWS_SOURCE)
#define FUNC_INLINE inline
#elif defined(_MSC_VER)
#define FUNC_INLINE __inline
#else
#define FUNC_INLINE
#endif

static FUNC_INLINE int
pmc_uconv_wrong_trail_byte(pmc_u8ch_t c)
{
  return ((c & 0xc0) != 0x80);
}

pmc_uconv_err_t
pmc_uconv_8to16(const pmc_u8ch_t *r8, const pmc_u8ch_t *const r8end,
  pmc_u16ch_t *w16, pmc_u16ch_t *const w16end,
  const pmc_u8ch_t **r8pos_r, pmc_u16ch_t **w16pos_r)
{
  pmc_uconv_err_t err = pmc_uconv_err_success;
  while (r8 < r8end) {
    pmc_u8ch_t c;
    c = r8[0];
    if (w16 >= w16end) {
      err = pmc_uconv_err_bufferfull;
      break;
    }
    if ((c & 0x80) == 0) {
      /* 0xxxxxxx 7bits */
      w16[0] = c;
      r8 += 1;
      w16 += 1;
    } else if ((c & 0xe0) == 0xc0) {
      /* 110xxxxx 10xxxxxx 11bits */
      pmc_u8ch_t c2;
      pmc_u16ch_t u;
      if (r8 + 1 >= r8end) {
	err = pmc_uconv_err_incomplete;
	break;
      }
      c2 = r8[1];
      u = ((c & 0x1f) << 6) | (c2 & 0x3f);
      if (u < 128) {
	err = pmc_uconv_err_range;
	break;
      }
      w16[0] = u;
      r8 += 2;
      w16 += 1;
    } else if ((c & 0xf0) == 0xe0) {
      /* 1110xxxx 10xxxxxx 10xxxxxx 16bits */
      pmc_u8ch_t c2, c3;
      pmc_u32ch_t u;
      if (r8 + 2 >= r8end) {
	err = pmc_uconv_err_incomplete;
	break;
      }
      c2 = r8[1];
      c3 = r8[2];
      if (pmc_uconv_wrong_trail_byte(c2) || pmc_uconv_wrong_trail_byte(c3)) {
	err = pmc_uconv_err_malformed;
	break;
      }
      u = ((c & 0x0f) << 12) | ((c2 & 0x3f) << 6) | (c3 & 0x3f);
      if (u < 2048 || (u >= 0xd800 && u < 0xe000)) {
	err = pmc_uconv_err_range;
	break;
      }
      w16[0] = u;
      r8 += 3;
      w16 += 1;
    } else if ((c & 0xf8) == 0xf0) {
      /* 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx 21bits */
      pmc_u8ch_t c2, c3, c4;
      pmc_u32ch_t u;
      if (w16 + 1 >= w16end) {
	err = pmc_uconv_err_bufferfull;
	break;
      }
      if (r8 + 3 >= r8end) {
	err = pmc_uconv_err_incomplete;
	break;
      }
      c2 = r8[1];
      c3 = r8[2];
      c4 = r8[3];
      if (pmc_uconv_wrong_trail_byte(c2) || pmc_uconv_wrong_trail_byte(c3)
	|| pmc_uconv_wrong_trail_byte(c4)) {
	err = pmc_uconv_err_malformed;
	break;
      }
      u = ((c & 0x07) << 18) | ((c2 & 0x3f) << 12) | ((c3 & 0x3f) << 6)
	| (c4 & 0x3f);
      if (u < 0x10000 || u > 0x10ffff) {
	err = pmc_uconv_err_range;
	break;
      }
      u -= 0x10000; /* [0 - 0xfffff] 20bits */
      w16[0] = (u >> 10) + 0xd800;
      w16[1] = (u & 0x3ff) + 0xdc00;
      r8 += 4;
      w16 += 2;
    } else {
      /* 11111xxx */
      err = pmc_uconv_err_malformed;
      break;
    }
  }
  (*r8pos_r) = r8;
  (*w16pos_r) = w16;
  return err;
}

pmc_uconv_err_t
pmc_uconv_16to8(const pmc_u16ch_t *r16, const pmc_u16ch_t *const r16end,
  pmc_u8ch_t *w8, pmc_u8ch_t *const w8end,
  const pmc_u16ch_t **r16pos_r, pmc_u8ch_t **w8pos_r)
{
  pmc_uconv_err_t err = pmc_uconv_err_success;
  while (r16 < r16end) {
    pmc_u16ch_t c;
    c = r16[0];
    if (w8 >= w8end) {
      err = pmc_uconv_err_bufferfull;
      break;
    }
    if (c < 0x80) {
      w8[0] = (pmc_u8ch_t)c;
      r16 += 1;
      w8 += 1;
    } else if (c < 0x800) {
      if (w8 + 1 >= w8end) {
	err = pmc_uconv_err_bufferfull;
	break;
      }
      w8[0] = (pmc_u8ch_t)(0xc0 | ((c >> 6) & 0x1f));
      w8[1] = (pmc_u8ch_t)(0x80 | (c & 0x3f));
      r16 += 1;
      w8 += 2;
    } else if (c >= 0xd800 && c < 0xdc00) {
      pmc_u16ch_t c2;
      pmc_u32ch_t u;
      if (w8 + 3 >= w8end) {
	err = pmc_uconv_err_bufferfull;
	break;
      }
      if (r16 + 1 >= r16end) {
	err = pmc_uconv_err_incomplete;
	break;
      }
      c2 = r16[1];
      if (c2 < 0xdc00 || c2 >= 0xe000) {
	err = pmc_uconv_err_malformed;
	break;
      }
      u = (((pmc_u32ch_t)c & 0x3ff) << 10) + (c2 & 0x3ff) + 0x10000;
      w8[0] = (pmc_u8ch_t)(0xf0 | ((u >> 18) & 0x7));
      w8[1] = (pmc_u8ch_t)(0x80 | ((u >> 12) & 0x3f));
      w8[2] = (pmc_u8ch_t)(0x80 | ((u >> 6) & 0x3f));
      w8[3] = (pmc_u8ch_t)(0x80 | (u & 0x3f));
      r16 += 2;
      w8 += 4;
    } else if (c >= 0xdc00 && c < 0xe000) {
      err = pmc_uconv_err_malformed;
      break;
    } else {
      if (w8 + 2 >= w8end) {
	err = pmc_uconv_err_bufferfull;
	break;
      }
      w8[0] = (pmc_u8ch_t)(0xe0 | ((c >> 12) & 0xf));
      w8[1] = (pmc_u8ch_t)(0x80 | ((c >> 6) & 0x3f));
      w8[2] = (pmc_u8ch_t)(0x80 | (c & 0x3f));
      r16 += 1;
      w8 += 3;
    }
  }
  (*r16pos_r) = r16;
  (*w8pos_r) = w8;
  return err;
}

#ifdef GWS_SOURCE
}; // namespace aioport
#endif

