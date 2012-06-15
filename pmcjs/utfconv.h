
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef AIOPORT_UTFCONV_HPP
#define AIOPORT_UTFCONV_HPP

#if defined(GWS_SOURCE)
namespace aioport {
#elif defined(__cplusplus)
extern "C" {
#endif

typedef enum {
  pmc_uconv_err_success = 0,
  pmc_uconv_err_malformed,
  pmc_uconv_err_range,
  pmc_uconv_err_incomplete,
  pmc_uconv_err_bufferfull,
} pmc_uconv_err_t;

typedef unsigned char pmc_u8ch_t;
typedef unsigned short pmc_u16ch_t;
typedef unsigned int pmc_u32ch_t;

pmc_uconv_err_t
pmc_uconv_8to16(const pmc_u8ch_t *r8, const pmc_u8ch_t *const r8end,
  pmc_u16ch_t *w16, pmc_u16ch_t *const w16end,
  const pmc_u8ch_t **r8pos_r, pmc_u16ch_t **w16pos_r);
pmc_uconv_err_t
pmc_uconv_16to8(const pmc_u16ch_t *r16, const pmc_u16ch_t *const r16end,
  pmc_u8ch_t *w8, pmc_u8ch_t *const w8end,
  const pmc_u16ch_t **r16pos_r, pmc_u8ch_t **w8pos_r);

#if defined(AIOPORT)
}; // namespace aioport
#elif defined(__cplusplus)
}
#endif

#endif

