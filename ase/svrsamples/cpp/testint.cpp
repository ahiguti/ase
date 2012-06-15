
#include <ase/asecclass.hpp>
#include <iostream>
#include <stdio.h>

#define DBG(x)

namespace {

struct testsvr {
  testsvr() : intval(0) {
    DBG(printf("testsvr %p\n", this));
  }
  ~testsvr() {
    DBG(printf("~testsvr %p\n", this));
  }
  void SetInt(int x) {
    intval = x;
    DBG(printf("s %p x=%d iv=%d\n", this, x, intval));
  }
  int GetInt() {
    DBG(printf("g %p iv=%d\n", this, intval));
    return intval;
  }
  void AddInt(int x) {
    intval += x;
    DBG(printf("a %p x=%d iv=%d\n", this, x, intval));
  }
  void AddInt2(int x) {
    intval += x;
    DBG(printf("a2 %p x=%d iv=%d\n", this, x, intval));
  }
  void SetStr(const ase_string& x) {
    strval = x;
  }
  void ObjRef(testsvr& o) const {
    printf("ObjRef: o=%d s=%d\n", o.intval, intval);
  }
  int intval;
  ase_string strval;
  ase_variant objval;
};

struct mklass {

  static void Main() {
    printf("mklass Main\n");
    ase_variant testsvrk = ase_cclass<testsvr>::get_class();
    ase_variant obj = testsvrk.minvoke("Create");
    obj.minvoke("SetInt", 3);
    ase_int id = obj.get_method_id("AddInt");
    printf("%d\n", id);
    for (int i = 0; i < 10000000; ++i) {
    // for (int i = 0; i < 10; ++i) {
      // obj.minvoke("AddInt", 1);
      obj.minvoke(id, 1);
    }
    std::cout << obj.minvoke("GetInt").get_int() << std::endl;
  }

};

}; // anonymous namespace

extern "C" ase_variant
ASE_DLLMain()
{
  ase_cclass<testsvr>::initializer()
    .def("SetInt", &testsvr::SetInt)
    .def("GetInt", &testsvr::GetInt)
    .def("AddInt", &testsvr::AddInt)
    .def("SetStr", &testsvr::SetStr)
    .def("ObjRef", &testsvr::ObjRef)
    .def("Create", &ase_cclass<testsvr>::create0)
    .def("AddInt2", &testsvr::AddInt2)
    ;
  ase_cclass<mklass>::initializer()
    .def("", &mklass::Main)
    ;
  return ase_cclass<mklass>::get_class();
}

