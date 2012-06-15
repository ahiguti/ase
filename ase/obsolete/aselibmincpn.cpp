
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asecpnt.hpp>

namespace {

struct component_minimal_klass {
  component_minimal_klass(const ase_string& n, ASE_Component::Library& lib) { }
  ase_variant hoge(const ase_variant *args, ase_size_type nargs) {
    return ase_variant();
  }
  ase_variant fuga(const ase_variant *args, ase_size_type nargs) {
    return ase_variant();
  }
  static const ASE_Component::MethodEntry<component_minimal_klass>
    ClassMethods[];
};

struct component_minimal {
  component_minimal(const component_minimal_klass& klass,
    const ase_variant *args, ase_size_type nargs) {
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
  static const ASE_Component::MethodEntry<component_minimal>
    InstanceMethods[];
};

const ASE_Component::MethodEntry<component_minimal_klass>
component_minimal_klass::ClassMethods[] = {
  { "hoge", &component_minimal_klass::hoge },
  { "fuga", &component_minimal_klass::fuga },
};
const ASE_Component::MethodEntry<component_minimal>
component_minimal::InstanceMethods[] = {
  { "foo", &component_minimal::foo },
  { "bar", &component_minimal::bar },
  { "baz", &component_minimal::baz },
};

}; // anonymous namespace

extern "C" void
ASE_NewVariantFactory_ComponentMinimal(ASE_VariantFactoryPtr& ap_r)
{
  static const ASE_Component::LibraryEntry ents[] = {
    {
      "Minimal",
      &ASE_Component::NewClass<
	component_minimal_klass, component_minimal, false
      >
    },
  };
  ASE_Component::NewLibraryFactory(ents, ap_r);
}

