
#include <ase/asecpnfobj.hpp>

#include <iostream>

namespace {

struct klass {

  ase_variant operator ()(ASE_Component::Library& lib,
    const ase_variant *args, ase_size_type nargs) {
    printf("a\n");
    ase_variant native = lib.GetGlobals().get("Native");
    printf("b\n");
    ase_variant testnative = native.get("ASETestNative.TestNative");
    printf("c\n");
    ase_variant obj = testnative.invoke();
    printf("d\n");
    obj.minvoke("SetInt", 3);
    printf("e\n");
    ase_int id = obj.GetMethodId("AddInt");
    printf("f %d\n", id);
    for (int i = 0; i < 10000000; ++i) {
      // obj.minvoke("AddInt", 1);
      obj.minvoke(id, 1);
    }
    printf("g\n");
    std::cout << obj.minvoke("get_int").get_int() << std::endl;
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

/*
local Test = Native['ASETestNative.TestNative']
local obj = Test()
obj.SetInt(3)
local i, r
for i = 1, 1000000 do
  obj.AddInt(1)
end
print(obj.get_int())
*/

