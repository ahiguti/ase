
// FIXME: これはテスト用なので消すべし

/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asecclass.hpp>
#include <ase/asestrmap.hpp>

struct Foo {
  Foo(int x) { }
  int bar(int x, int y) {
    return x + y;
  }
  std::string sc(std::string x, std::string y) {
    return "STR(" + x + y + ")";
  }
  std::string sc2(const std::string& x, const std::string& y) {
    return "STR(" + x + y + ")";
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
};

struct dummy {
  dummy() {
    {
      ase_cclass<Foo>::initializer()
	.def("m2", &Foo::m2)
	.def("sc", &Foo::sc)
	.def("sc2", &Foo::sc2)
	.def("met1", &Foo::met1)
	.def("fubar", &Foo::fubar)
	.def("ba", &Foo::bar)
	.def("sbar", &Foo::sbar)
	.def("New", &ase_cclass<Foo>::Create1<int>)
	;
      // std::printf("oitest\n");
      ase_variant x = ase_cclass<Foo>::Create1(3);
      ase_variant x2 = x;
      ase_variant z = x.minvoke("fubar", 5, 3);
      ase_variant w = x.minvoke("sc", "FOO", "BAR");
      std::printf("oitest: %d %s\n", z.get_int(), w.get_string().data());
      ase_variant c = ase_cclass<Foo>::GetClass();
      ase_variant k = c.minvoke("sbar", 10, 20);
      std::printf("oktest: %d\n", k.get_int());
      ase_variant lib = ase_new_string_map();
      lib.set("Foo", ase_cclass<Foo>::GetClass());
    }
    ase_cclass_base::Terminate();
  }
};

static dummy dummyobj;

