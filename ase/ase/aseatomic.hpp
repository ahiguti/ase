
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASECPPATOMIC_HPP
#define GENTYPE_ASECPPATOMIC_HPP

typedef long ase_atomic_type;

#if __GNUC__ >= 4 && (defined(__x86_64__) || defined(__i386__))

struct ase_atomic {
  typedef ase_atomic_type value_type;
  static long locked_xadd(value_type& x, int v) {
    return __sync_fetch_and_add(&x, v);
  }
  static long get(const value_type& x) { return x; }
  static void set(value_type& x, long v) { x = v; }
};

#elif defined(__GNUC__)

#include <string>
#include <bits/atomicity.h>

struct ase_atomic {
  static long locked_xadd(ase_atomic_type& x, int v) {
    using namespace __gnu_cxx;
    _Atomic_word *ptr = (_Atomic_word *)(&x); /* NOTE: size truncated */
    return __exchange_and_add(ptr, v);
  }
  static long get(const ase_atomic_type& x) {
    const _Atomic_word *ptr = (const _Atomic_word *)(&x);
    return *ptr;
  }
  static void set(ase_atomic_type& x, long v) {
    _Atomic_word *ptr = (_Atomic_word *)(&x);
    (*ptr) = v;
  }
};

#elif defined(_MSC_VER)

#include <windows.h>

struct ase_atomic {
  static long locked_xadd(ase_atomic_type& x, int v) {
    return InterlockedExchangeAdd(&x, v);
  }
  static long get(const ase_atomic_type& x) { return x; }
  static void set(ase_atomic_type& x, long v) { x = v; }
};

#else

#error "need to define ase_atomic"

#endif

class ase_atomic_count {

 public:

  ase_atomic_count(int v) { ase_atomic::set(value, v); }
  ase_atomic_count(const ase_atomic_count& x) : value(x) { }
  int operator ++(int) {
    return ase_atomic::locked_xadd(value, 1);
  }
  int operator --(int) {
    return ase_atomic::locked_xadd(value, -1);
  }
  operator int() const { return ase_atomic::get(value); }

 private:

  ase_atomic_type value;

};

#endif

