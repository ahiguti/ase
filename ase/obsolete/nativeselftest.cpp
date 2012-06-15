
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asecpnfobj.hpp>

namespace {

struct selftest {

  ase_variant operator ()(ASE_Component::Library& lib,
    const ase_variant *args, ase_size_type nargs) {
    ase_variant luaeval = lib.GetGlobals().get("Lua").get("EvalString");
    ase_variant fsrc("return function(x) return x + x end");
    ase_variant fobj = luaeval.invoke(fsrc);
    ase_variant x(120);
    ase_variant r = fobj.invoke(x);
    std::cout << "retval=" << r << std::endl;
    return r;
  }

}; 

}; // anonymous namespace

extern "C" void
ASE_NewVariantFactory_SelfTest(ASE_VariantFactoryPtr& ap_r)
{
  ASE_ComponentFuncObject::NewLibraryFactory<selftest, false>(ap_r);
}

