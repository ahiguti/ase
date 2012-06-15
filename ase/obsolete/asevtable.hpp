
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEVTABLE_HPP
#define GENTYPE_ASEVTABLE_HPP

#include <ase/asecpp.hpp>

class ASE_VTable : public ase_noncopyable {

 public:

  ASELIB_DLLEXT ASE_VTable(const ase_variant_vtable& base,
    ase_size_type num_methods);
  ASELIB_DLLEXT ~ASE_VTable(); /* DCHK */

  const ase_variant_vtable& get() const { return vtable; }
  ASELIB_DLLEXT ase_variant_method_entry& Method(ase_size_type i);
  ASELIB_DLLEXT void SetInstanceVTable(const ase_variant_vtable *instance);

 private:

  struct impl_type;
  impl_type *impl;
  ase_variant_vtable vtable;

};

#endif

