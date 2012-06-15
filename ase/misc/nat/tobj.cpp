
#include <ase/asecpnfobj.hpp>

namespace {

struct klass {

  ase_variant operator ()(ASE_Component::Library& lib,
    const ase_variant *args, ase_size_type nargs) {
    ase_unified_engine_ref& ue = lib.GetUnifiedEngineRef();
    ase_variant ntestmod = ue.LoadFile(ase_string("ntest.cpp"));
    ase_variant testnative = ntestmod.get("TestNative");
    ase_int i = 0;
    for (i = 0; i < 10000000; ++i) {
      ase_variant obj = testnative.invoke();
      obj.minvoke("SetInt", i);
    }
    std::cout << "val=" << i << std::endl;
    return ase_variant();

  }

}; 

}; // anonymous namespace

extern "C" {
ASE_COMPAT_DLLEXPORT void
ASE_NewVariantFactory(ASE_VariantFactoryPtr& ap_r)
{
  ASE_ComponentFuncObject::NewLibraryFactory<klass, false>(ap_r);
}
};

