
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asecclass.hpp>
#include <ase/asestrmap.hpp>

struct Foo {
  Foo(int x) : ival(x) {
    std::printf("Foo constr\n");
  }
  int bar(int x, int y) {
    return x + y;
  }
  std::string sc(std::string x, std::string y) {
    return "STR(" + x + y + ")";
  }
  std::string sc2(const std::string& x, const std::string& y) {
    return "STR(" + x + y + ")";
  }
  void obj(const Foo& another) {
    std::printf("obj ano=%d self=%d", another.ival, ival);
  }
  int fubar(int x, int y) {
    return x * y;
  }
  int m2(int x) {
    return x * 2;
  }
  void met1(int x, int y) {
  }
  static int sbar(int x, int y) {
    return x - y;
  }
  int ival;
};

extern "C" ase_variant
ASE_DLLMain()
{
  ase_cclass<Foo>::initializer()
    .def("m2", &Foo::m2)
    .def("sc", &Foo::sc)
    .def("sc2", &Foo::sc2)
    .def("met1", &Foo::met1)
    .def("fubar", &Foo::fubar)
    .def("ba", &Foo::bar)
    .def("sbar", &Foo::sbar)
    .def("obj", &Foo::obj)
    .def("Create", &ase_cclass<Foo>::Create1<int>)
    ;
  std::printf("dllmain foo\n");
  ase_variant r = ase_cclass<Foo>::GetClass();
  std::printf("r=%s\n", r.get_string().data());
    {
      ase_variant c = ase_cclass<Foo>::GetClass();
      ase_variant k = c.minvoke("sbar", 10, 20);
      std::printf("oktest: %d\n", k.get_int());
    }
  return r;
}

