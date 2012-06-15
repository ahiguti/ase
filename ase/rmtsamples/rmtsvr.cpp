
#include <ase/asecclass.hpp>
#include <stdio.h>
#define DBG(x)

struct testsvr {
  testsvr() : intval(0) {
    DBG(printf("testsvr\n"));
  }
  void SetInt(int x) {
    intval = x;
    DBG(printf("s %p x=%d iv=%d\n", this, x, intval));
  }
  int GetInt() {
    DBG(printf("g %p iv=%d\n", this, intval));
    return intval;
  }
  int Sum(const ase_variant& arr) {
    int r = 0;
    const int n = arr.get_length();
    for (int i = 0; i < n; ++i) {
      r += arr.get(i).get_int();
    }
    DBG(printf("Sum %p r=%d\n", this, r));
    return r;
  }
  int intval;
};

extern "C" {

ASE_COMPAT_DLLEXPORT ase_variant
ASE_DLLMain()
{
  DBG(printf("dllmain\n"));
  ase_cclass<testsvr>::initializer()
    .def("SetInt", &testsvr::SetInt)
    .def("GetInt", &testsvr::GetInt)
    .def("Sum", &testsvr::Sum)
    .def("Create", &ase_cclass<testsvr>::create0)
    ;
  return ase_cclass<testsvr>::get_class();
}

};
