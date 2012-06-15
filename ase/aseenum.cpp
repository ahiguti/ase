
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/aseenum.hpp>
#include <ase/asetyped.hpp>

#include <vector>

namespace {

struct ase_enum_varimpl : public ase_variant_impl_typed<ase_enum_varimpl> {

  ase_enum_varimpl(const ase_string *ks, ase_size_type nks) : pos(0) {
    keys.insert(keys.end(), ks, ks + nks);
  }
  ~ase_enum_varimpl() /* DCHK */ { }

  static ase_string get_string(const ase_variant& v) {
    return ase_string("[ASEEnum]");
  }
  static ase_string enum_next(const ase_variant& v, bool& hasnext_r) {
    ase_enum_varimpl *selfp = get(v);
    if (selfp->pos >= selfp->keys.size()) {
      hasnext_r = false;
      return ase_string();
    }
    hasnext_r = true;
    return selfp->keys[selfp->pos++];
  }

  std::vector<ase_string> keys;
  ase_size_type pos;
  static const char *const debugid;

};

const char *const ase_enum_varimpl::debugid = "Enum";

}; // anonymous namespace

ase_variant
ase_new_enum(const ase_string *keys, ase_size_type nkeys)
{
  return ase_variant::create_object(
    &ase_variant_impl<ase_enum_varimpl>::vtable,
    new ase_enum_varimpl(keys, nkeys),
    ase_enum_varimpl::debugid);
}

