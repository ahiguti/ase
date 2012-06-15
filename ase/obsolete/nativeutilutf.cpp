
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asecpnt.hpp>
#include <ase/aseutf16.hpp>

namespace {

struct utilutf {

  utilutf(const ase_string& n, ASE_Component::Library& lib) { }
  utilutf(const utilutf& klass, const ase_variant *args, ase_size_type nargs) {
  }
  ase_variant Noop(const ase_variant *args, ase_size_type nargs) {
    return ase_variant();
  }

  static const ASE_Component::MethodEntry<utilutf> ClassMethods[];
  static const ASE_Component::MethodEntry<utilutf> InstanceMethods[];

};

const ASE_Component::MethodEntry<utilutf> utilutf::ClassMethods[] = {
  { "Noop", &utilutf::Noop },
};
const ASE_Component::MethodEntry<utilutf> utilutf::InstanceMethods[] = {
  { "Noop", &utilutf::Noop },
};

}; // anonymous namespace

ase_variant
ASE_NewUtilUTFClass(const ase_string& name, ASE_Component::Library& lib)
{
  return ASE_Component::NewClass<utilutf, utilutf, true>(name, lib);
}

