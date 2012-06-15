
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEALGO_HPP
#define GENTYPE_ASEALGO_HPP

#include <ase/ase.hpp>

template <typename Iter, typename Val, typename Compare> inline Iter
ase_binary_search(Iter first, Iter last, Iter notfound, Val val,
  Compare compare)
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

template <typename Iter, typename Op> inline void
ase_for_each(Iter first, Iter last, Op op)
{
  for ( ; first != last; ++first) {
    op(*first);
  }
}

template <typename Iter, typename Op1, typename Op2> inline void
ase_for_each(Iter first, Iter last, Op1 cnstr, Op2 dstr)
{
  Iter i = first;
  try {
    for ( ; i != last; ++i) {
      cnstr(*i);
    }
  } catch (...) {
    ase_for_each(first, i, dstr);
    throw;
  }
}

#endif

