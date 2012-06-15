
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/aseasn1.hpp>
#include <ase/asearray.hpp>
#include <ase/asestrmap.hpp>

#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <stdexcept>
#include "aseasn1/aseremote_impl.hpp"

#ifndef _MSC_VER
/* strncasecmp */
#include <strings.h>
#endif

ase_string
ase_asn1der_serialize(const ase_variant *start, const ase_variant *finish,
  ase_object_serializer *om)
{
  aseremote::ase_remote_variant rv;
  aseremote::to_remote_variant(start, finish, om, rv);
  const std::string derstr = rv.get_der_stdstring();
  return ase_string(derstr);
}

ase_string
ase_asn1der_serialize_value(const ase_variant& value)
{
  aseremote::ase_remote_variant rv;
  aseremote::to_remote_variant(value, 0, rv);
  const std::string derstr = rv.get_der_stdstring();
  return ase_string(derstr);
}

ase_string
ase_asn1der_serialize_object_op(int op, bool is_remote, ase_long id)
{
  aseremote::ase_remote_variant rv;
  rv.set_array();
  aseremote::ase_remote_variant x1;
  x1.set_int(op);
  rv.push_back(x1);
  aseremote::ase_remote_variant x2;
  x2.set_object(is_remote, id);
  rv.push_back(x2);
  const std::string derstr = rv.get_der_stdstring();
  return ase_string(derstr);
}

ase_variant
ase_asn1der_deserialize(const char *str, ase_size_type slen,
  ase_object_deserializer *ou)
{
  aseremote::ase_remote_variant rv;
  if (!rv.set_from_ber(str, slen)) {
    return ase_variant();
  }
  return aseremote::from_remote_variant(rv.get(), ou);
}

ase_variant
ase_asn1der_deserialize_value(const char *str, ase_size_type slen)
{
  return ase_asn1der_deserialize(str, slen, 0);
}

bool
ase_asn1der_deserialize_get_length(const char *str, ase_size_type slen,
  ase_size_type& len_r, bool& overflow_r)
{
  overflow_r = false;
  const unsigned char *ustr = reinterpret_cast<const unsigned char *>(str);
  if (slen < 2) {
    return false;
  }
  const unsigned char lc = ustr[1];
  if ((lc & 0x80U) == 0) {
    /* short form */
    len_r = 2 + lc;
    return true;
  }
  const ase_size_type len = lc & 0x7fU;
  size_t tl_len = len + 2;
  if (len > sizeof(ase_size_type)) {
    overflow_r = true;
    return true;
  }
  if (slen < len + 2) {
    return false;
  }
  ase_size_type vl = 0;
  for (ase_size_type i = 0; i < len; ++i) {
    const unsigned char uv = ustr[i + 2];
    vl <<= 8;
    vl |= uv;
  }
  len_r = tl_len + vl;
  return true;
}

ase_string
ase_asn1xer_serialize(const ase_variant *start, const ase_variant *finish,
  ase_object_serializer *om)
{
  aseremote::ase_remote_variant rv;
  aseremote::to_remote_variant(start, finish, om, rv);
  const std::string xerstr = rv.get_xer_stdstring();
  return ase_string(xerstr);
}

ase_string
ase_asn1xer_serialize_value(const ase_variant& value)
{
  aseremote::ase_remote_variant rv;
  aseremote::to_remote_variant(value, 0, rv);
  const std::string xerstr = rv.get_xer_stdstring();
  return ase_string(xerstr);
}

ase_string
ase_asn1xer_serialize_object_op(int op, bool is_remote, ase_long id)
{
  aseremote::ase_remote_variant rv;
  rv.set_array();
  aseremote::ase_remote_variant x1;
  x1.set_int(op);
  rv.push_back(x1);
  aseremote::ase_remote_variant x2;
  x2.set_object(is_remote, id);
  rv.push_back(x2);
  const std::string xerstr = rv.get_xer_stdstring();
  return ase_string(xerstr);
}

ase_variant
ase_asn1xer_deserialize(const char *str, ase_size_type slen,
  ase_object_deserializer *ou)
{
  aseremote::ase_remote_variant rv;
  if (!rv.set_from_xer(str, slen)) {
    return ase_variant();
  }
  return aseremote::from_remote_variant(rv.get(), ou);
}

ase_variant
ase_asn1xer_deserialize_value(const char *str, ase_size_type slen)
{
  return ase_asn1xer_deserialize(str, slen, 0);
}

bool
ase_asn1xer_deserialize_get_length(const char *str, ase_size_type slen,
  ase_size_type& len_r, bool& overflow_r)
{
  len_r = 0;
  overflow_r = false;
  const char *const eom = "</ASERemoteVariant>";
  const ase_size_type eom_len = std::strlen(eom);
  if (slen < eom_len) {
    return false;
  }
  for (ase_size_type i = 0; i <= slen - eom_len; ++i) {
#ifdef _MSC_VER
    if (_strnicmp(str + i, eom, eom_len) == 0)
#else
    if (::strncasecmp(str + i, eom, eom_len) == 0)
#endif
    {
      len_r = i + eom_len;
      return true;
    }
  }
  return false;
}

namespace {

const ase_serializer_table serializer_table_asn1der = {
  ase_asn1der_serialize,
  ase_asn1der_serialize_object_op,
  ase_asn1der_deserialize,
  ase_asn1der_deserialize_get_length,

};

const ase_serializer_table serializer_table_asn1xer = {
  ase_asn1xer_serialize,
  ase_asn1xer_serialize_object_op,
  ase_asn1xer_deserialize,
  ase_asn1xer_deserialize_get_length,

};

};

const ase_serializer_table *
ase_serializer_table_asn1der()
{
  return &serializer_table_asn1der;
}

const ase_serializer_table *
ase_serializer_table_asn1xer()
{
  return &serializer_table_asn1xer;
}

