
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEASN1_HPP
#define GENTYPE_ASEASN1_HPP

#include <ase/aseserialize.hpp>

ASELIB_DLLEXT ase_string ase_asn1der_serialize(const ase_variant *start,
  const ase_variant *finish, ase_object_serializer *om);
ASELIB_DLLEXT ase_string ase_asn1der_serialize_value(const ase_variant& value);
ASELIB_DLLEXT ase_string ase_asn1der_serialize_object_op(int op,
  bool is_remote, ase_long id);
ASELIB_DLLEXT ase_variant ase_asn1der_deserialize(const char *str,
  ase_size_type slen, ase_object_deserializer *ou);
ASELIB_DLLEXT ase_variant ase_asn1der_deserialize_value(const char *str,
  ase_size_type slen);
ASELIB_DLLEXT bool ase_asn1der_deserialize_get_length(const char *str,
  ase_size_type slen, ase_size_type& len_r, bool& overflow_r);
  /* returns false if the length part is incomplete */

ASELIB_DLLEXT ase_string ase_asn1xer_serialize(const ase_variant *start,
  const ase_variant *finish, ase_object_serializer *om);
ASELIB_DLLEXT ase_string ase_asn1xer_serialize_value(const ase_variant& value);
ASELIB_DLLEXT ase_string ase_asn1xer_serialize_object_op(int op,
  bool is_remote, ase_long id);
ASELIB_DLLEXT ase_variant ase_asn1xer_deserialize(const char *str,
  ase_size_type slen, ase_object_deserializer *ou);
ASELIB_DLLEXT ase_variant ase_asn1xer_deserialize_value(const char *str,
  ase_size_type slen);
ASELIB_DLLEXT bool ase_asn1xer_deserialize_get_length(const char *str,
  ase_size_type slen, ase_size_type& len_r, bool& overflow_r);
  /* returns false if the length part is incomplete */

ASELIB_DLLEXT const ase_serializer_table *ase_serializer_table_asn1der();
ASELIB_DLLEXT const ase_serializer_table *ase_serializer_table_asn1xer();

#endif

