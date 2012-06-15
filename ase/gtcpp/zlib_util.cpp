
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/zlib_util.hpp>
#include <gtcpp/string_util.hpp>
#include <boost/cast.hpp>
#include <stdexcept>
#include <cstring>
#include <zlib.h>

#define DBG(x)

namespace gtcpp {


void
zlib_compress(string_buffer& outbuf, const char *start,
  const char *finish, int level)
{
  const size_t srclen = finish - start;
  unsigned char lenbuf[8];
  size_t l = srclen;
  for (size_t i = 0; i < 8; ++i) {
    lenbuf[7 - i] = l & 0xffU;
    l >>= 8;
  }
  if (l != 0) {
    throw std::runtime_error("zlib_compress: integer overflow");
  }
  outbuf.append(reinterpret_cast<char *>(lenbuf), 8);
  uLong destlen = compressBound(srclen);
  DBG(fprintf(stderr, "compress destlen=%lu\n", destlen));
  char *wrptr = outbuf.make_space(destlen);
  Bytef *const dest = reinterpret_cast<Bytef *>(wrptr);
  const int r = compress2(dest, &destlen,
    reinterpret_cast<const Bytef *>(start),
    boost::numeric_cast<uLong>(srclen), level);
  if (r != Z_OK) {
    throw std::runtime_error("zlib_compress: failed");
  }
  DBG(fprintf(stderr, "compress actual destlen=%lu\n", destlen));
  outbuf.append_done(destlen);
}

void
zlib_uncompress(string_buffer& outbuf, const char *start, const char *finish)
{
  if (finish - start < 8) {
    throw std::runtime_error("zlib_uncompress: too short");
  }
  const size_t srclen = finish - start - 8;
  const unsigned char *const lenbuf =
    reinterpret_cast<const unsigned char *>(start);
  size_t l = 0;
  for (size_t i = 0; i < 8; ++i) {
    const size_t ln = l << 8;
    if (ln >> 8 != l) {
      throw std::runtime_error("zlib_uncompress: integer overflow");
    }
    l = ln | lenbuf[i];
  }
  uLong destlen = boost::numeric_cast<uLong>(l);
  DBG(fprintf(stderr, "uncompress destlen=%lu\n", destlen));
  char *wrptr = outbuf.make_space(destlen);
  Bytef *const dest = reinterpret_cast<Bytef *>(wrptr);
  const int r = uncompress(dest, &destlen,
    reinterpret_cast<const Bytef *>(start + 8),
    boost::numeric_cast<uLong>(srclen));
  if (r != Z_OK) {
    throw std::runtime_error("zlib_uncompress: failed");
  }
  DBG(fprintf(stderr, "uncompress actual destlen=%lu\n", destlen));
  outbuf.append_done(destlen);
}

};


