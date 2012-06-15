
#include <ase/asecclass.hpp>

#include <stdio.h>

#define DBG(x)

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
  void SetStr(const ase_string& x) {
    strval = x;
  }
  void ObjRef(testsvr& o) const {
    printf("ObjRef: o=%d s=%d\n", o.intval, intval);
  }
  void ObjCRef(const testsvr& o) const {
    printf("ObjCRef: o=%d s=%d\n", o.intval, intval);
  }
  void VariantRef(ase_variant& v) {
    printf("VRef: s=%s\n", v.get_string().data());
  }
  void VariantCRef(const ase_variant& v) {
    printf("VCRef: s=%s\n", v.get_string().data());
  }
  static ase_variant SFunc(const ase_variant *args, ase_size_type nargs) {
    ase_int r = 0;
    for (ase_size_type i = 0; i < nargs; ++i) {
      r += args[i].get_int();
    }
    return r;
  }
  ase_variant IFunc(const ase_variant *args, ase_size_type nargs) {
    return SFunc(args, nargs).get_int() * intval;
  }
  int intval;
  ase_string strval;
  ase_variant objval;
};

extern "C" {

ASE_COMPAT_DLLEXPORT ase_variant
ASE_DLLMain()
{
  ase_cclass<testsvr>::initializer()
    .def("SetInt", &testsvr::SetInt)
    .def("GetInt", &testsvr::GetInt)
    .def("AddInt", &testsvr::AddInt)
    .def("SetStr", &testsvr::SetStr)
    .def("SFunc", &testsvr::SFunc)
    .def("ObjRef", &testsvr::ObjRef)
    .def("ObjCRef", &testsvr::ObjCRef)
    .def("VariantRef", &testsvr::VariantRef)
    .def("VariantCRef", &testsvr::VariantCRef)
    .def("IFunc", &testsvr::IFunc)
    .def("Create", &ase_cclass<testsvr>::create0)
    ;
  return ase_cclass<testsvr>::get_class();
}

};
