
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_ATOMICITY_HPP
#define GTCPP_ATOMICITY_HPP

#include <string.h>

#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 3) || __GNUC__ >= 5

namespace gtcpp {
inline int atomic_xadd(volatile int& x, int value)
{
  return __sync_fetch_and_add(&x, value);
}
};

#else

#include <bits/atomicity.h>
namespace gtcpp {
inline int atomic_xadd(volatile int& x, int value)
{
  #if ( __GNUC__ == 3 && __GNUC_MINOR__ >= 4 ) || __GNUC__ >= 4
  using namespace __gnu_cxx;
  #endif
  return __exchange_and_add(&x, value);
}
};

#endif

#endif

