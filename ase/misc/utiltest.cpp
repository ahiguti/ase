
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asecpnfobj.hpp>

namespace {

struct utiltest {

  ase_variant operator ()(ASE_Component::Library& lib,
    const ase_variant *args, ase_size_type nargs) {

    typedef ase_variant Var;

    Var Test = lib.GetGlobals().get("Native").get("Util").get("Test");
    ase_int mid_sum = Test.GetMethodId("Sum");
    ase_variant r;
    for (int i = 0; i < 10000000; ++i) {
      // r = Test.minvoke("Sum", 10, 20, 30);
      r = Test.minvoke(mid_sum, 10, 20, 30);
    }
    std::cout << r << std::endl;
    return r;

  }

}; 

}; // anonymous namespace

extern "C" void
ASE_NewVariantFactory(ASE_VariantFactoryPtr& ap_r)
{
  ASE_ComponentFuncObject::NewLibraryFactory<utiltest, false>(ap_r);
}

