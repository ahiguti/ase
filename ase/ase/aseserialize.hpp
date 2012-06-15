
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASESERIALIZE_HPP
#define GENTYPE_ASESERIALIZE_HPP

#include <ase/ase.hpp>

struct ase_object_serializer {

  virtual ~ase_object_serializer() { }
  virtual bool serialize_object(const ase_variant& value, bool& remote_r,
    ase_long& id_r) = 0;

};

struct ase_object_deserializer {

  virtual ~ase_object_deserializer() { }
  virtual ase_variant deserialize_object(bool remote, ase_long id) = 0;

};

struct ase_serializer_table {
  ase_string (*serialize)(const ase_variant *start, const ase_variant *finish,
    ase_object_serializer *om);
  ase_string (*serialize_object_op)(int op, bool is_remote, ase_long obj_id);
  ase_variant (*deserialize)(const char *str, ase_size_type slen,
    ase_object_deserializer *ou);
  bool (*deserialize_get_length)(const char *str, ase_size_type slen,
    ase_size_type& len_r, bool& overflow_r);
};

ASELIB_DLLEXT ase_string ase_serialize_compact(const ase_variant *start,
  const ase_variant *finish, ase_object_serializer *om);
ASELIB_DLLEXT ase_string ase_serialize_compact_object_op(int op,
  bool is_remote, ase_long obj_id);
ASELIB_DLLEXT ase_variant ase_deserialize_compact(const char *str,
  ase_size_type slen, ase_object_deserializer *ou);
ASELIB_DLLEXT bool ase_deserialize_compact_get_length(const char *str,
  ase_size_type slen, ase_size_type& len_r, bool& overflow_r);
  /* returns false if the length part is incomplete */
ASELIB_DLLEXT const ase_serializer_table *ase_serializer_table_default();

#endif

