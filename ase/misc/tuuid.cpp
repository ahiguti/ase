
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asecpnfobj.hpp>

namespace {

struct uuidtest {

  ase_variant operator ()(ASE_Component::Library& lib,
    const ase_variant *args, ase_size_type nargs) {

    typedef ase_variant Var;

    Var UUID = lib.GetGlobals().get("Native").get("Util").get("UUID");
    for (int i = 0; i < 10; ++i) {
      ase_variant x = UUID.invoke();
      std::cout << x.minvoke("upper") << std::endl;
    }
    return Var();

  }

}; 

}; // anonymous namespace

extern "C" {
ASE_COMPAT_DLLEXPORT void
ASE_NewVariantFactory(ASE_VariantFactoryPtr& ap_r)
{
  ASE_ComponentFuncObject::NewLibraryFactory<uuidtest, false>(ap_r);
}
};
