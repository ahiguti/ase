
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/auto_iconv.hpp>
#include <gtcpp/posix_error.hpp>
#include <errno.h>

namespace gtcpp {

auto_iconv::auto_iconv(const char *tocode, const char *fromcode)
  : conv(invalid_iconv())
{
  conv = iconv_open(tocode, fromcode);
  if (conv == invalid_iconv() && errno != EINVAL) {
    throw_posix_error(errno, __PRETTY_FUNCTION__);
  }
}

auto_iconv::~auto_iconv()
{
  if (conv != invalid_iconv()) {
    iconv_close(conv);
  }
}

void
iconv(iconv_t cd, string_buffer& buf, const char *start,
  const char *finish)
{
  while (start < finish) {
    const size_t wblocksize = 1024;
    char *const wp = buf.make_space(wblocksize);
    char *inbuf = const_cast<char *>(start);
    size_t inbytesleft = finish - start;
    char *outbuf = wp;
    size_t outbytesleft = wblocksize;
    const size_t icr = ::iconv(cd, &inbuf, &inbytesleft, &outbuf,
      &outbytesleft);
    const int e = errno;
    if (icr == auto_iconv::iconv_failed()) {
      switch (e) {
      case EILSEQ:
	/* invalid multibyte sequence */
	if (inbytesleft > 0) {
	  ++inbuf;
	  --inbytesleft;
	}
	break;
      case EINVAL:
	/* invalid multibyte sequence at the end of the input string */
	inbytesleft = 0;
	break;
      case E2BIG:
	/* no more space to write */
	break;
      default:
	throw_posix_error(e, __PRETTY_FUNCTION__);
	break;
      }
    } else {
      /* success */
    }
    const char *const n_start = finish - inbytesleft;
    start = n_start;
    buf.append_done(outbuf - wp);
  }
}

bool
iconv(const char *tocode, const char *fromcode, string_buffer& outbuf,
  const char *start, const char *finish)
{
  auto_iconv cv(tocode, fromcode);
  if (!cv) {
    return false;
  }
  iconv(cv.get(), outbuf, start, finish);
  return true;
}

};

