
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/aseintermed.hpp>
#include <ase/asetyped.hpp>
#include <ase/aseatomic.hpp>
#include <ase/asestrutil.hpp>

namespace {

class vimpl_intermed : public ase_variant_impl_typed<vimpl_intermed> {

 public:

  vimpl_intermed(const ase_variant& v, const ase_string& n)
    : value(v), name(n) { }
  static ase_string get_string(const ase_variant& self) {
    return ase_string("[ASEIntermed]");
  }
  static ase_variant get_property(const ase_variant& self, const char *n,
    ase_size_type nlen) {
    vimpl_intermed *selfp = get(self);
    return selfp->value.get_property(selfp->name + ase_string(n, nlen));
  }
  static const char *const debugid;

 private:

  const ase_variant value;
  const ase_string name;

};

const char *const vimpl_intermed::debugid = "IntermedProp";

}; // anonymous namespace

ase_variant
ase_new_intermediate_property(const ase_variant& val, const ase_string& name)
{
  if ((val.get_vtoption() & ase_vtoption_multithreaded) != 0) {
    /* inherit unlock flag */
    return ase_variant::create_object(
      &ase_variant_impl<
	vimpl_intermed,
	ase_vtoption_multithreaded
      >::vtable,
      new vimpl_intermed(val, name),
      vimpl_intermed::debugid);
  } else {
    return ase_variant::create_object(
      &ase_variant_impl<vimpl_intermed>::vtable,
      new vimpl_intermed(val, name),
      vimpl_intermed::debugid);
  }
}

ase_variant
ase_get_property_chained(const ase_variant& val, const char *name,
  ase_size_type namelen, char delim)
{
  ase_variant v = val;
  while (true) {
    if (namelen == 0) {
      return v;
    }
    const char *const p = ase_strfind(name, namelen, delim);
    if (!p) {
      return v.get_property(name, namelen);
    }
    ase_size_type plen = (p - name);
    v = v.get_property(name, plen);
    name += (plen + 1);
    namelen -= (plen + 1);
  }
}

