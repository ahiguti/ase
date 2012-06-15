
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASETYPED_HPP
#define GENTYPE_ASETYPED_HPP

#include <ase/ase.hpp>
#include <ase/aseatomic.hpp>

class ase_variant_impl_typed_base : public ase_variant_impl_default {

 public:

  ase_variant_impl_typed_base() { }

  ASELIB_DLLEXT static ase_variant get_property(const ase_variant& v,
    const char *name, ase_size_type len);
  ASELIB_DLLEXT static ase_variant get_element(const ase_variant& v,
    ase_int idx);
  ASELIB_DLLEXT static ase_variant invoke_by_name(const ase_variant& v,
    const char *name, ase_size_type namelen, const ase_variant *args,
    ase_size_type nargs);

};

template <typename T>
class ase_variant_impl_typed : public ase_variant_impl_typed_base {

 public:

  ase_variant_impl_typed() : refcount(1) { }

  static void add_ref(const ase_variant& v) {
    get(v)->refcount++;
  }
  static void release(const ase_variant& v) {
    T *const p = get(v);
    if ((p->refcount--) == 1) {
      ase_variant::debug_on_destroy_object(v, T::debugid);
      delete p;
    }
  }

 protected:

  static T *get(const ase_variant& v) {
    return static_cast<T *>(v.get_rep().p);
  }

 private:

  ase_atomic_count refcount;

};

#endif

