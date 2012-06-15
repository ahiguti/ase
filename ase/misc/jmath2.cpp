
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asecpnfobj.hpp>
#include <ase/asefuncobj.hpp>

namespace {

struct cppvariant {

  ase_variant operator ()(ASE_Component::Library& lib,
    const ase_variant *args, ase_size_type nargs) {

    ase_variant Math = lib.GetGlobals().get("Java").get("java.lang.Math");
    ase_variant r;
    ase_variant x(-99.0);
    ase_int id = Math.GetMethodId("abs");
    for (int i = 0; i < 1000000; ++i) {
      r = Math.minvoke(id, x);
      // r = Math.minvoke("abs", x);
    }
    std::cout << "id=" << id << std::endl;
    std::cout << "r=" << r << std::endl;

    return r;
  }

}; 

}; // anonymous namespace

extern "C" void
ASE_NewVariantFactory(ASE_VariantFactoryPtr& ap_r)
{
  ASE_ComponentFuncObject::NewLibraryFactory<cppvariant, false>(ap_r);
}

