
#include <ase/asecclass.hpp>
#include <ase/aseasn1.hpp>
#include <ase/asearray.hpp>

#include <stdio.h>

#define DBG(x)

struct xmltest {
  static ase_string ToXML(const ase_variant& v) {
    return ase_asn1xer_serialize_value(v);
  }
  static ase_variant FromXML(const ase_string& s) {
    return ase_asn1xer_deserialize_value(s.data(), s.size());
  }
  static ase_string ToDER(const ase_variant& v) {
    return ase_asn1der_serialize_value(v);
  }
  static ase_variant FromDER(const ase_string& s) {
    return ase_asn1der_deserialize_value(s.data(), s.size());
  }
  static ase_variant ToSerial(const ase_variant *args, ase_size_type nargs) {
    return ase_serialize_compact(args, args + nargs, 0).to_variant();
  }
  static ase_variant FromSerial(const ase_string& s) {
    return ase_deserialize_compact(s.data(), s.size(), 0);
  }
  static void to_hex(std::string& s, unsigned char c) {
    s.push_back(c >= 10 ? (c - 10 + 'a') : (c + '0'));
  }
  static ase_string ToHEX(const ase_string& s) {
    std::string r;
    for (ase_size_type i = 0; i < s.size(); ++i) {
      const unsigned char c = static_cast<unsigned char>(s.data()[i]);
      to_hex(r, c >> 4);
      to_hex(r, c & 0x0fU);
    }
    return ase_string(r);
  }
  static ase_variant DERLen(const ase_string& s) {
    ase_size_type derlen = 0;
    bool ov = false;
    bool suc = ase_asn1der_deserialize_get_length(s.data(), s.size(), derlen,
      ov);
    const ase_variant r = ase_new_array();
    r.set_element(0, suc);
    r.set_element(1, static_cast<ase_long>(derlen));
    r.set_element(2, ov);
    return r;
  }
  static ase_variant SerialLen(const ase_string& s) {
    ase_size_type len = 0;
    bool ov = false;
    bool suc = ase_deserialize_compact_get_length(s.data(), s.size(), len, ov);
    const ase_variant r = ase_new_array();
    r.set(0, suc);
    r.set(1, static_cast<ase_long>(len));
    r.set_element(2, ov);
    return r;
  }
};

extern "C" {

ASE_COMPAT_DLLEXPORT ase_variant
ASE_DLLMain()
{
  ase_cclass<xmltest>::initializer()
    .def("ToXML", &xmltest::ToXML)
    .def("FromXML", &xmltest::FromXML)
    .def("ToDER", &xmltest::ToDER)
    .def("FromDER", &xmltest::FromDER)
    .def("ToHEX", &xmltest::ToHEX)
    .def("DERLen", &xmltest::DERLen)
    .def("ToSerial", &xmltest::ToSerial)
    .def("FromSerial", &xmltest::FromSerial)
    .def("SerialLen", &xmltest::SerialLen)
    ;
  return ase_cclass<xmltest>::get_class();
}

};
