
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/aseutf16.hpp>
#include <ase/aseexcept.hpp>

#include "aseutfconv.hpp"

#define DBG_CONV(x)

namespace {

bool
is_trailing_byte(pmc_u8ch_t c)
{
  return ((c & 0xc0) == 0x80);
}

ase_ucchar *
create_u16buffer(const char *str, ase_size_type len, ase_size_type& len16_r,
  bool permissive, pmc_uconv_err_t& err_r)
{
  ase_ucchar *const buffer = static_cast<ase_ucchar *>(
    std::malloc((len + 1) * sizeof(ase_ucchar)));
  if (!buffer) {
    return buffer;
  }
  ase_ucchar *start_u16c = buffer;
  ase_ucchar *const finish_u16c = start_u16c + len;
  const pmc_u8ch_t *start_u8c = (const pmc_u8ch_t *)(str);
  const pmc_u8ch_t *const finish_u8c = start_u8c + len;
  while (true) {
    err_r = pmc_uconv_8to16(start_u8c, finish_u8c, start_u16c,
      finish_u16c, &start_u8c, &start_u16c);
    if ((!permissive) || err_r == pmc_uconv_err_bufferfull
      || err_r == pmc_uconv_err_success) {
      break;
    }
    (*start_u16c) = 0xfffdU;
    start_u16c += 1;
    start_u8c += 1;
    while (start_u8c < finish_u8c && is_trailing_byte(*start_u8c)) {
      ++start_u8c;
    }
    err_r = pmc_uconv_err_success;
  }
  (*start_u16c) = 0;
  len16_r = (start_u16c - buffer);
  return buffer;
}

}; // anonymous namespace

void
ase_utf16_string::init(const char *str, ase_size_type len, bool permissive)
{
  /* NOTE: this func is called from constr and we must free buffer before
    throwing an exception */
  pmc_uconv_err_t err = pmc_uconv_err_success;
  buffer = create_u16buffer(str, len, length, permissive, err);
  if (!buffer) {
    throw std::bad_alloc();
  }
  if (permissive && err != pmc_uconv_err_bufferfull) {
    /* don't ignore bufferfull for safety */
    err = pmc_uconv_err_success;
  }
  if (err != pmc_uconv_err_success) {
    ase_variant ex = ase_new_exception(ase_string("UTF16Conv"),
      "(ASERuntime) failed to convert to utf16: " +
      ase_string(pmc_uconv_error_string(err)));
    DBG_CONV(fprintf(stderr, "throwing exc: %s\n", ex.get_string().data()));
    std::free(buffer);
    buffer = 0;
    throw ex;
  }
}

ase_utf16_string::ase_utf16_string(const char *str, ase_size_type len,
  const permissive&)
  : length(0), buffer(0)
{
  init(str, len, true);
}

ase_utf16_string::ase_utf16_string(const char *str, ase_size_type len)
  : length(0), buffer(0)
{
  init(str, len, false);
}

ase_utf16_string::ase_utf16_string(const ase_string& str)
  : length(0), buffer(0)
{
  init(str.data(), str.size(), false);
}

ase_utf16_string::ase_utf16_string(const std::string& str)
  : length(0), buffer(0)
{
  init(str.data(), str.size(), false);
}

ase_utf16_string::~ase_utf16_string() /* DCHK */
{
  std::free(buffer);
}

