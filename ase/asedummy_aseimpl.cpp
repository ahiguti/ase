
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/aseflags.hpp>

#include "asedummy_util.hpp"
#include "asedummy_aseimpl.hpp"

#define DBG(x)

namespace asedummy {

ase_dummy_ase_variant_impl::ase_dummy_ase_variant_impl(const init_arg& arg)
  : refcount(1)
{
  ASE_DEBUG(ase_dummy_variant_impl_count_constr++);
}

ase_dummy_ase_variant_impl::~ase_dummy_ase_variant_impl() /* DCHK */
{
  ASE_DEBUG(ase_dummy_variant_impl_count_destr++);
}

void
ase_dummy_ase_variant_impl::add_ref(const ase_variant& self)
{
  ase_dummy_ase_variant_implnode *const selfp = get(self);
  selfp->value.refcount++;
}

void
ase_dummy_ase_variant_impl::release(const ase_variant& self)
{
  ase_dummy_ase_variant_implnode *const selfp = get(self);
  if ((selfp->value.refcount--) == 1) {
    ase_dummy_script_engine *const sc = getctx(self);
    ase_dummy_script_engine::callfromase_guard g(sc);
    selfp->value.dispose(sc);
    ase_variant::debug_on_destroy_object(self, "ComObject");
    delete selfp;
  }
}

void
ase_dummy_ase_variant_impl::dispose(ase_dummy_script_engine *sc)
{
}

ase_int
ase_dummy_ase_variant_impl::get_attributes(const ase_variant& self)
{
  return 0;
}

bool
ase_dummy_ase_variant_impl::get_boolean(const ase_variant& self)
{
  return false;
}

ase_int
ase_dummy_ase_variant_impl::get_int(const ase_variant& self)
{
  return 0;
}

ase_long
ase_dummy_ase_variant_impl::get_long(const ase_variant& self)
{
  return 0;
}

double
ase_dummy_ase_variant_impl::get_double(const ase_variant& self)
{
  return 0.0;
}

ase_string
ase_dummy_ase_variant_impl::get_string(const ase_variant& self)
{
  return ase_string("[DummyObject]");
}

bool
ase_dummy_ase_variant_impl::is_same_object(const ase_variant& self,
  const ase_variant& value)
{
  return false;
}

ase_variant
ase_dummy_ase_variant_impl::get_property(const ase_variant& self,
  const char *name, ase_size_type namelen)
{
  return ase_variant();
}

void
ase_dummy_ase_variant_impl::set_property(const ase_variant& self,
  const char *name, ase_size_type namelen, const ase_variant& value)
{
}

void
ase_dummy_ase_variant_impl::del_property(const ase_variant& self,
  const char *name, ase_size_type namelen)
{
}

ase_variant
ase_dummy_ase_variant_impl::get_element(const ase_variant& self, ase_int index)
{
  return ase_variant();
}

void
ase_dummy_ase_variant_impl::set_element(const ase_variant& self, ase_int index,
  const ase_variant& value)
{
}

ase_int
ase_dummy_ase_variant_impl::get_length(const ase_variant& self)
{
  return -1;
}

void
ase_dummy_ase_variant_impl::set_length(const ase_variant& self, ase_int len)
{
}

ase_variant
ase_dummy_ase_variant_impl::invoke_by_name(const ase_variant& self,
  const char *name, ase_size_type namelen, const ase_variant *args,
  ase_size_type nargs)
{
  ase_variant pv = get_property(self, name, namelen);
  return pv.invoke_self(args, nargs);
}

ase_variant
ase_dummy_ase_variant_impl::invoke_by_id(const ase_variant& self, ase_int id,
  const ase_variant *args, ase_size_type nargs)
{
  return ase_variant();
}

ase_variant
ase_dummy_ase_variant_impl::invoke_self(const ase_variant& self,
  const ase_variant *args, ase_size_type nargs)
{
  return ase_variant();
}

ase_variant
ase_dummy_ase_variant_impl::get_enumerator(const ase_variant& self)
{
  return ase_variant();
}

ase_string
ase_dummy_ase_variant_impl::enum_next(const ase_variant& self, bool& hasnext_r)
{
  hasnext_r = false;
  return ase_string();
}

}; // namespace asedummy

