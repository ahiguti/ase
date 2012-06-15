
#include <ase/asecclass.hpp>
#include <ase/aseasn1.hpp>
#include <ase/asearray.hpp>

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
  void ObjCRef(const testsvr& o) const {
    printf("ObjCRef: o=%d s=%d\n", o.intval, intval);
  }
  void VariantRef(ase_variant& v) {
    printf("VRef: s=%s\n", v.get_string().data());
  }
  void VariantCRef(const ase_variant& v) {
    printf("VCRef: s=%s\n", v.get_string().data());
  }
  static ase_string ToDER(const ase_variant& v) {
    return ase_asn1der_serialize_value(v);
  }
  static ase_variant FromDER(const ase_string& s) {
    return ase_asn1der_deserialize_value(s.data(), s.size());
  }
  static ase_string ToDERLoop(const ase_variant& v, int loop) {
    ase_string s;
    for (int i = 0; i < loop; ++i) {
      s = ase_asn1der_serialize_value(v);
    }
    return s;
  }
  static ase_variant FromDERLoop(const ase_string& s, int loop) {
    ase_variant v;
    for (int i = 0; i < loop; ++i) {
      v = ase_asn1der_deserialize_value(s.data(), s.size());
    }
    return v;
  }
  static ase_string ToDERLoopTest(int loop) {
    ase_variant a = ase_new_array();
    a.set_element(0, ase_variant(10));
    a.set_element(1, ase_variant(20));
    a.set_element(2, ase_variant(30));
    ase_string s;
    for (int i = 0; i < loop; ++i) {
      s = ase_asn1der_serialize_value(a);
    }
    return s;
  }
  int intval;
  ase_string strval;
  ase_variant objval;
};

extern "C" {

ASE_COMPAT_DLLEXPORT ase_variant
ASE_DLLMain()
{
  const ase_int opt = 0 /* ase_vtoption_multithreaded */;
  ase_cclass<testsvr>::initializer(opt)
    .def("SetInt", &testsvr::SetInt)
    .def("GetInt", &testsvr::GetInt)
    .def("AddInt", &testsvr::AddInt)
    .def("SetStr", &testsvr::SetStr)
    .def("ObjRef", &testsvr::ObjRef)
    .def("ObjCRef", &testsvr::ObjCRef)
    .def("VariantRef", &testsvr::VariantRef)
    .def("VariantCRef", &testsvr::VariantCRef)
    .def("Create", &ase_cclass<testsvr>::create0)
    .def("AddInt2", &testsvr::AddInt2)
    .def("ToDER", &testsvr::ToDER)
    .def("FromDER", &testsvr::FromDER)
    .def("ToDERLoop", &testsvr::ToDERLoop)
    .def("FromDERLoop", &testsvr::FromDERLoop)
    .def("ToDERLoopTest", &testsvr::ToDERLoopTest)
    ;
  return ase_cclass<testsvr>::get_class();
}

};
