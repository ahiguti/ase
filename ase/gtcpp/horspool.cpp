
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/horspool.hpp>
#include <cstring>

namespace gtcpp {

void
horspool_create_shift(const char *needle_signed, unsigned int needle_len,
  horspool_shift_map& shift)
{
  const unsigned char *const needle =
    reinterpret_cast<const unsigned char *>(needle_signed);
  for (unsigned int i = 0; i < 256; ++i) {
    shift[i] = needle_len;
  }
  for (unsigned int i = 0; i < needle_len - 1; ++i) {
    shift[needle[i]] = needle_len - 1 - i;
  }
}

const char *
horspool_search(const char *needle_signed, unsigned int needle_len,
  const horspool_shift_map& shift, const char *haystack_signed,
  unsigned int haystack_len)
{
  const unsigned char *const needle =
    reinterpret_cast<const unsigned char *>(needle_signed);
  const unsigned char *haystack =
    reinterpret_cast<const unsigned char *>(haystack_signed);
  const unsigned int last = needle_len - 1;
  while (haystack_len >= needle_len) {
    const unsigned char needle_last = needle[last];
    const unsigned char haystack_last = haystack[last];
    if (needle_last == haystack_last &&
      std::memcmp(needle, haystack, last) == 0) {
      return reinterpret_cast<const char *>(haystack);
    }
    const unsigned int s = shift[haystack_last];
    haystack += s;
    haystack_len -= s;
  }
  return 0;
}

const char *
horspool_search(const char *needle, unsigned int needle_len,
  const char *haystack, unsigned int haystack_len)
{
  horspool_shift_map shift;
  horspool_create_shift(needle, needle_len, shift);
  return horspool_search(needle, needle_len, shift, haystack, haystack_len);
}

};

