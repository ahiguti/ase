
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asevtable.hpp>

#include <memory>
#include <vector>

struct ASE_VTable::impl_type {
  std::vector<ase_variant_method_entry> methods;
};

ASE_VTable::ASE_VTable(const ase_variant_vtable& base,
  ase_size_type num_methods)
  : impl(0), vtable(base)
{
  std::auto_ptr<impl_type> im(new impl_type());
  im->methods.resize(num_methods ? num_methods : 1);
  for (ase_size_type i = 0; i < num_methods; ++i) {
    im->methods[i].u8name = 0;
    im->methods[i].u8name_len = 0;
  }
  vtable.methods.methods = &im->methods[0];
  vtable.methods.num_methods = num_methods;
  impl = im.release();
}

ASE_VTable::~ASE_VTable() /* DCHK */
{
  delete impl;
}

ase_variant_method_entry&
ASE_VTable::Method(ase_size_type i)
{
  return impl->methods[i];
}

void
ASE_VTable::SetInstanceVTable(const ase_variant_vtable *instance)
{
  vtable.instance = instance;
}

