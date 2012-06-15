
#include <ase/asecpnfobj.hpp>
#include <vector>

namespace {

struct klass {

  ase_variant operator ()(ASE_Component::Library& lib,
    const ase_variant *args, ase_size_type nargs) {
    ase_unified_engine_ref& ue = lib.GetUnifiedEngineRef();
    ase_variant ntestmod = ue.LoadFile(ase_string("ntest.cpp"));
    ase_variant testnative = ntestmod.get("TestNative");
    ase_int i = 0;
    std::vector<ase_variant> arr(1000000);
    for (i = 0; i < 1000000; ++i) {
      ase_variant obj = testnative.invoke();
      obj.minvoke("SetInt", i * 2);
      arr[i] = obj;
    }
    ase_variant obj = arr[23];
    ase_variant x = obj.minvoke("get_int");
    std::cout << "val=" << x.get_int() << std::endl;

    /* dummy loop */
    #if 0
    for (i = 0; i < 10000000; ++i) {
      obj = arr[i / 10];
      x = obj.get_int();
      obj.minvoke("SetInt", x.get_int() * 3);
    }
    #endif
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

