
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_ZLIB_UTIL_HPP
#define GTCPP_ZLIB_UTIL_HPP

#include <gtcpp/string_buffer.hpp>

namespace gtcpp {

void zlib_compress(string_buffer& outbuf, const char *start,
  const char *finish, int level = 1);
void zlib_uncompress(string_buffer& outbuf, const char *start,
  const char *finish);

};

#endif

