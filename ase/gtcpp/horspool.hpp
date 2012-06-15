
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_HORSPOOL_HPP
#define GTCPP_HORSPOOL_HPP

namespace gtcpp {

typedef unsigned int horspool_shift_map[256];

/* TODO: haystack_len should be of type size_t */

void horspool_create_shift(const char *needle,
  unsigned int needle_len, horspool_shift_map& shift);
const char *horspool_search(const char *needle, unsigned int needle_len,
  const horspool_shift_map& shift, const char *haystack,
  unsigned int haystack_len); /* returns 0 if not found */
const char *horspool_search(const char *needle, unsigned int needle_len,
  const char *haystack, unsigned int haystack_len);
  /* returns 0 if not found */

};

#endif

