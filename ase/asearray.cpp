
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asearray.hpp>
#include <ase/asetyped.hpp>
#include <ase/asemutex.hpp>
#include <ase/aseexcept.hpp>

#include <vector>

#define DBG(x)

namespace {

struct ase_array_varimpl : public ase_variant_impl_typed<ase_array_varimpl> {

  typedef std::vector<ase_variant> array_type;
  typedef array_type::size_type size_type;

  ase_array_varimpl(ase_size_type len) : array(len) {
  }

  ase_array_varimpl(const ase_variant *vs, ase_size_type len)
    : array(vs, vs + len) {
  }

  static ase_string get_string(const ase_variant& v) {
    return ase_string("[ASEArray]");
  }
  static ase_variant get_property(const ase_variant& v,
    const char *name, ase_size_type namelen) {
    ase_array_varimpl *selfp = get(v);
    if (ase_string::equals(name, namelen, "Length")) {
      return ase_variant(static_cast<ase_int>(selfp->array.size()));
    }
    return ase_variant();
  }
  static ase_variant get_element(const ase_variant& v, ase_int idx) {
    ase_array_varimpl *selfp = get(v);
    ase_mutex_guard<ase_null_mutex> g(selfp->mutex);
    if (idx < 0 || static_cast<size_type>(idx) >= selfp->array.size()) {
      return ase_variant();
    }
    DBG(fprintf(stderr, "getelem %d\n", idx));
    return selfp->array[static_cast<size_type>(idx)];
  }
  static void set_element(const ase_variant& v, ase_int idx,
    const ase_variant& value) {
    ase_array_varimpl *selfp = get(v);
    ase_mutex_guard<ase_null_mutex> g(selfp->mutex);
    if (idx < 0) {
      return;
    }
    size_type uidx = static_cast<size_type>(idx);
    if (uidx >= selfp->array.size()) {
      selfp->array.resize(uidx + 1);
    }
    DBG(fprintf(stderr, "setelem %d %s\n", idx,
      value.get_string().data()));
    selfp->array[static_cast<size_type>(idx)] = value;
  }
  static ase_int get_length(const ase_variant& v) {
    ase_array_varimpl *selfp = get(v);
    ase_mutex_guard<ase_null_mutex> g(selfp->mutex);
    return static_cast<ase_int>(selfp->array.size());
  }
  static void set_length(const ase_variant& v, ase_int len) {
    ase_array_varimpl *selfp = get(v);
    ase_mutex_guard<ase_null_mutex> g(selfp->mutex);
    if (len < 0) {
      len = 0;
    }
    selfp->array.resize(static_cast<size_type>(len));
  }

  ase_null_mutex mutex;
  array_type array;
  static const char *const debugid;

};

const char *const ase_array_varimpl::debugid = "Array";

struct ase_array_unmod_varimpl : public ase_array_varimpl {

  static void set_element(const ase_variant& v, ase_int idx,
    const ase_variant& value) {
    ase_throw_illegal_operation("ASEArray: unmodifiable");
  }
  static void set_length(const ase_variant& v, ase_int len) {
    ase_throw_illegal_operation("ASEArray: unmodifiable");
  }

};

}; // anonymous namespace

ase_variant
ase_new_array(ase_size_type len)
{
  return ase_variant::create_object(
    &ase_variant_impl<ase_array_varimpl, ase_vtoption_remote_by_value>::vtable,
    new ase_array_varimpl(len),
    ase_array_varimpl::debugid);
}

ase_variant
ase_new_array(const ase_variant *vs, ase_size_type len)
{
  return ase_variant::create_object(
    &ase_variant_impl<ase_array_varimpl, ase_vtoption_remote_by_value>::vtable,
    new ase_array_varimpl(vs, len),
    ase_array_varimpl::debugid);
}

ase_variant
ase_array_unmodifiable(const ase_variant& m)
{
  const void *xvt = m.get_vtable_address();
  const ase_variant_vtable *vtbl = 0;
  if (xvt ==
    &ase_variant_impl<ase_array_varimpl, ase_vtoption_remote_by_value>
      ::vtable) {
    vtbl = &ase_variant_impl<ase_array_unmod_varimpl,
      ase_vtoption_remote_by_value>::vtable;
  } else {
    ase_throw_type_mismatch("ASEArray: not an array object");
  }
  ase_array_varimpl *av = static_cast<ase_array_varimpl *>(m.get_rep().p);
  av->add_ref(m);
  return ase_variant::create_object(vtbl, av, 0);
}

