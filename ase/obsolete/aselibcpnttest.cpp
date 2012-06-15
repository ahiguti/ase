
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asecpnt.hpp>

#define DBG(x)
#define DBG_SIZE(x)
#define DBG_COUNT(x)

namespace {

unsigned long objcount = 0;
unsigned long klscount = 0;

struct component_testclass {

  component_testclass(const ase_string& n, ASE_Component::Library& lib) {
    DBG_COUNT(klscount++);
    DBG_COUNT(fprintf(stderr, "klscount=%d\n", (int)klscount));
  }
  ~component_testclass() /* DCHK */ {
    klscount--;
    DBG_COUNT(fprintf(stderr, "klscount=%d\n", (int)objcount));
  }
  ase_variant hoge(const ase_variant *args, ase_size_type nargs) {
    return ase_variant();
  }
  ase_variant fuga(const ase_variant *args, ase_size_type nargs) {
    return ase_variant();
  }

  static const ASE_Component::MethodEntry<component_testclass> ClassMethods[];

};

struct component_test {

  component_test(const component_testclass& klass, const ase_variant *args,
    ase_size_type nargs) {
    value = nargs > 0 ? args[0] : ase_variant();
    DBG_COUNT(objcount++);
    DBG_COUNT(fprintf(stderr, "count=%d\n", (int)objcount));
  }
  ~component_test() /* DCHK */ {
    objcount--;
    DBG_COUNT(fprintf(stderr, "count=%d\n", (int)objcount));
  }
  ase_variant abs(const ase_variant *args, ase_size_type nargs) {
    ase_int r = 0;
    if (nargs >= 1) {
      r = std::abs(args[0].get_int());
    }
    return ase_variant::create_int(r);
  }
  ase_variant foo(const ase_variant *args, ase_size_type nargs) {
    return ase_variant();
  }
  ase_variant bar(const ase_variant *args, ase_size_type nargs) {
    return ase_variant();
  }
  ase_variant baz(const ase_variant *args, ase_size_type nargs) {
    return ase_variant();
  }

  ase_variant value;

  static const ASE_Component::MethodEntry<component_test> InstanceMethods[];

};

const ASE_Component::MethodEntry<component_testclass>
component_testclass::ClassMethods[] = {
  { "hoge", &component_testclass::hoge },
  { "fuga", &component_testclass::fuga },
};
const ASE_Component::MethodEntry<component_test>
component_test::InstanceMethods[] = {
  { "foo", &component_test::foo },
  { "bar", &component_test::bar },
  { "baz", &component_test::baz },
  { "abs", &component_test::abs },
};

}; // anonymous namespace

extern "C" {
void
ASE_NewVariantFactory(ASE_VariantFactoryPtr& ap_r)
{
  static const ASE_Component::LibraryEntry ents[] = {
    { "Test", ASE_Component::NewClass<
      component_testclass, component_test, false> },
  };
  ASE_Component::NewLibraryFactory(ents, ap_r);
}
};
