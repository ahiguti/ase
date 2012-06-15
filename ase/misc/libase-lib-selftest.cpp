
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asecpnfobj.hpp>
#include <ase/asefuncobj.hpp>

namespace {

struct selftest {

  struct cppfunc {
    cppfunc(const std::string& mess) : message(mess) { }
    ase_variant operator ()(const ase_variant *args, ase_size_type nargs) {
      std::cout << "cppfunc called: " << message << std::endl;
      return ase_variant();
    }
    std::string message;
  };

  ase_variant operator ()(ASE_Component::Library& lib,
    const ase_variant *args, ase_size_type nargs) {

    ase_variant luaeval = lib.GetGlobals().get("Lua").get("EvalString");
    ase_variant fobj = luaeval.invoke("return function(x) return x + x end");
    ase_variant r = fobj.invoke(120);
    std::cout << "retval=" << r << std::endl;

    ase_variant fo = ase_new_function_object(cppfunc("CPP"));
    fo.invoke();

    return r;
  }

}; 

}; // anonymous namespace

extern "C" void
ASE_NewVariantFactory(ASE_VariantFactoryPtr& ap_r)
{
  ASE_ComponentFuncObject::NewLibraryFactory<selftest, false>(ap_r);
}

