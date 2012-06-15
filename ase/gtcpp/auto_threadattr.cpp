
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/auto_threadattr.hpp>
#include <gtcpp/posix_error.hpp>

namespace gtcpp {

auto_threadattr::auto_threadattr()
{
  int e = pthread_attr_init(&attr);
  if (e != 0) {
    throw_posix_error(e, __PRETTY_FUNCTION__);
  }
}

auto_threadattr::~auto_threadattr()
{
  int e = pthread_attr_destroy(&attr);
  assert(e == 0);
  static_cast<void>(e); /* unused */
}

void
threadattr_setstacksize(auto_threadattr& attr, size_t stacksize)
{
  int e = pthread_attr_setstacksize(&attr.get(), stacksize);
  if (e != 0) {
    throw_posix_error(e, __PRETTY_FUNCTION__);
  }
}

};

