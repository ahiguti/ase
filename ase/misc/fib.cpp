
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asecpnfobj.hpp>

namespace {

int fib(int x)
{
  return x <= 1 ? 1 : fib(x - 2) + fib(x - 1);
}

struct utiltest {

  ase_variant operator ()(ASE_Component::Library& lib,
    const ase_variant *args, ase_size_type nargs) {

    int n = 30;
    if (nargs > 0) {
      n = std::atoi(args[0].get_string().data());
    }
    int r = fib(n);

    std::cout << r << std::endl;
    return 0;

  }

}; 

}; // anonymous namespace

extern "C" void
ASE_NewVariantFactory(ASE_VariantFactoryPtr& ap_r)
{
  ASE_ComponentFuncObject::NewLibraryFactory<utiltest, false>(ap_r);
}

