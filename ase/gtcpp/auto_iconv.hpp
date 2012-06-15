
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_AUTO_ICONV_HPP
#define GTCPP_AUTO_ICONV_HPP

#include <gtcpp/string_buffer.hpp>
#include <boost/noncopyable.hpp>
#include <iconv.h>

namespace gtcpp {

class auto_iconv : private boost::noncopyable {

 public:

  auto_iconv(const char *tocode, const char *fromcode);
  ~auto_iconv();
  operator bool() const { return conv != invalid_iconv(); }
  iconv_t get() const { return conv; }

  static iconv_t invalid_iconv() { return reinterpret_cast<iconv_t>(-1); } 
  static size_t iconv_failed() { return static_cast<size_t>(-1); }

 private:

  iconv_t conv;

};

void iconv(iconv_t cd, string_buffer& outbuf, const char *start,
  const char *finish);
/* fails only if iconv_open fails */
bool iconv(const char *tocode, const char *fromcode, string_buffer& outbuf,
  const char *start, const char *finish);

};

#endif

