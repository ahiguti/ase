
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

    ase_variant obj = lib.GetGlobals().get("Native").get("ComponentTest")
      .get("Test").invoke();
    ase_variant r;
    ase_variant x(-10);
    ase_int id = obj.GetMethodId("abs");
    for (int i = 0; i < 10000000; ++i) {
      r = obj.minvoke(id, x);
      // r = obj.minvoke("abs", x);
    }

    std::cout << r << std::endl;

    return r;
  }

}; 

}; // anonymous namespace

extern "C" void
ASE_NewVariantFactory(ASE_VariantFactoryPtr& ap_r)
{
  ASE_ComponentFuncObject::NewLibraryFactory<cppvariant, false>(ap_r);
}

