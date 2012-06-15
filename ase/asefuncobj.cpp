
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asefuncobj.hpp>

struct ase_function_object::ase_funcobj_varimpl
  : public ase_variant_impl_default {

  static funcobj_base *get(const ase_variant& self) {
    return static_cast<funcobj_base *>(self.get_rep().p);
  }

  static void add_ref(const ase_variant& self) {
    funcobj_base *selfp = get(self);
    selfp->refcount++;
  }
  static void release(const ase_variant& self) {
    funcobj_base *selfp = get(self);
    if ((selfp->refcount--) == 1) {
      ase_variant::debug_on_destroy_object(self, "FuncObj");
      delete selfp;
    }
  }
  static ase_string get_string(const ase_variant& self) {
    return ase_string("[ASEFuncObj]");
  }
  static ase_variant invoke_self(const ase_variant& self,
    const ase_variant *args, ase_size_type nargs) {
    funcobj_base *selfp = get(self);
    return selfp->call(args, nargs);
  }

};

ase_variant ase_function_object::create_internal(funcobj_base *ptr)
{
  return ase_variant::create_object(
    &ase_variant_impl<ase_funcobj_varimpl>::vtable,
    ptr,
    "FuncObj");
}

