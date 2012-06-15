
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/auto_glob.hpp>

namespace gtcpp {

auto_glob::auto_glob()
{
  globbuf.gl_pathc = 0;
  globbuf.gl_pathv = 0;
  globbuf.gl_offs = 0;
}

auto_glob::~auto_glob()
{
  reset();
}

void
auto_glob::reset()
{
  if (globbuf.gl_pathv) {
    ::globfree(&globbuf);
  }
  globbuf.gl_pathc = 0;
  globbuf.gl_pathv = 0;
  globbuf.gl_offs = 0;
}

int
glob(auto_glob& gl, const char *pattern, int flags, glob_error_callback& ec)
{
  if (gl.globbuf.gl_pathv) {
    if ((flags & GLOB_APPEND) == 0) {
      gl.reset();
    }
  } else {
    flags &= ~GLOB_APPEND;
  }
  int r = ::glob(pattern, flags, 0, &gl.globbuf);
  if (r != 0) {
    ec.on_glob_error(r, __PRETTY_FUNCTION__);
  }
  return r;
}

void
throw_glob_error(int e, const char *func)
{
  const char *mess;
  switch (e) {
  case GLOB_NOSPACE:
    mess = "NoSpace";
    break;
  case GLOB_ABORTED:
    mess = "Aborted";
    break;
  case GLOB_NOMATCH:
    mess = "NoMatch";
    break;
  default:
    mess = "UnknownGlobError";
  }
  throw glob_error(e, "GlobError", mess, func);
}

};

