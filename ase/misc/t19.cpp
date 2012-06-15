
#include <ase/asecpnfobj.hpp>

namespace {

struct fobj {

  ase_variant operator ()(ASE_Component::Library& lib,
    const ase_variant *args, ase_size_type nargs) {

    typedef ase_variant Var;

    Var Test = lib.GetGlobals().get("Native").get("Util").get("Test");
    ase_int mid_sum = Test.GetMethodId("Sum");
    ase_int mid_add = Test.get_instance_method_id("Add");
    ase_variant r;
    for (int i = 0; i < 10000000; ++i) {
      ase_variant obj = Test.invoke();
      obj.minvoke(mid_add, 1);
    }
    r = Test.minvoke(mid_sum, 10, 20, 30);
    std::cout << r << std::endl;
    return r;

  }

}; 

}; // anonymous namespace

extern "C" void
ASE_NewVariantFactory(ASE_VariantFactoryPtr& ap_r)
{
  ASE_ComponentFuncObject::NewLibraryFactory<fobj, false>(ap_r);
}

