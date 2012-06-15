
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asetyped.hpp>
#include <ase/asebindmethod.hpp>

namespace {

static bool match_method(const ase_variant_method_entry& ent,
  const char *name, ase_size_type len) {
  if (ent.u8name_len != len) {
    return false;
  }
  return std::memcmp(ent.u8name, name, len) == 0;
}

}; // anonymous namespace

ase_variant
ase_variant_impl_typed_base::get_property(const ase_variant& v,
  const char *name, ase_size_type len)
{
  const ase_variant_method_entry *mtds = v.get_methods();
  const ase_size_type num = v.get_num_methods();
  for (ase_size_type i = 0; i < num; ++i) {
    if (match_method(mtds[i], name, len)) {
      return ase_new_variant_bind_method_id(v, static_cast<ase_int>(i));
    }
  }
  return ase_variant();
}

ase_variant
ase_variant_impl_typed_base::get_element(const ase_variant& v, ase_int idx)
{
  const ase_size_type num = v.get_num_methods();
  if (idx >= 0 && static_cast<ase_size_type>(idx) < num) {
    return ase_new_variant_bind_method_id(v, static_cast<ase_int>(idx));
  }
  return ase_variant();
}

ase_variant
ase_variant_impl_typed_base::invoke_by_name(const ase_variant& v,
  const char *name, ase_size_type namelen, const ase_variant *args,
  ase_size_type nargs)
{
  const ase_variant_method_entry *mtds = v.get_methods();
  const ase_size_type num = v.get_num_methods();
  for (ase_size_type i = 0; i < num; ++i) {
    if (match_method(mtds[i], name, namelen)) {
      return v.invoke_by_id(static_cast<ase_int>(i), args, nargs);
    }
  }
  return v.get_property(name, namelen).invoke_self(args, nargs);
}

