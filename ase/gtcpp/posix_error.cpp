
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <string.h> /* GNU version of strerror_r */

#include <algorithm>
#include <gtcpp/posix_error.hpp>
#include <boost/scoped_array.hpp>
#include <vector>

namespace gtcpp {

void
throw_posix_error(int e, const char *func)
{
  char buf[256];
  char *p = strerror_r(e, buf, sizeof(buf)); /* GNU */
  throw posix_error(e, "PosixError", p, func);
}

void
posix_error_callback::on_posix_error(int e, const char *func)
{
  throw_posix_error(e, func);
}

};

