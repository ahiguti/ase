
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_ALGORITHM_HPP
#define GTCPP_ALGORITHM_HPP

namespace gtcpp {

template <typename Iter, typename Val, typename Compare> inline Iter
binary_search(Iter first, Iter last, Iter notfound, Val val, Compare compare)
{
  if (first == last) {
    return notfound;
  }
  --last;
  while (first <= last) {
    Iter mid = first + (last - first) / 2;
    int cv = compare(*mid, val);
    if (cv < 0) {
      first = mid + 1;
    } else if (cv > 0) {
      last = mid - 1;
    } else {
      return mid;
    }
  }
  return notfound;
}

};

#endif

