
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <cstdio>

struct C0 {
  static void func() {
    printf("C0::func\n");
  }
};

struct C1 : public C0 {
  static void func() {
    printf("C1::func\n");
  }
};

struct C2 : public C0 {
};

struct vtbl {
  void (*funcp)();
};

template <typename T> struct Tmpl {
  static void wrfunc() {
    T::func();
  }
  static vtbl vt;
};

#if 0
template <typename T> vtbl
Tmpl<T>::vt = {
  wrfunc
};
#endif

template <typename T> vtbl
Tmpl<T>::vt = {
  (&T::func) == (&C0::func) ? Tmpl<C0>::wrfunc : wrfunc
};

int
main() {
  Tmpl<C0>::vt.funcp();
  Tmpl<C1>::vt.funcp();
  Tmpl<C2>::vt.funcp();
  return 0;
}

