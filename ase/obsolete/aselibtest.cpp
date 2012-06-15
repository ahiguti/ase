
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asecpp.hpp>
#include <ase/asetyped.hpp>
#include <ase/aseweakref.hpp>
#include <ase/aseexcept.hpp>

#include <stdexcept>
#include <cstring>
#include <iostream>

namespace {

const ase_variant_method_entry impllt_methods[] = {
  { "foo", 3 },
  { "bar", 3 },
  { "abs", 3 },
  { "loop", 4 },
};

struct ASE_VariantImplLT : public ase_variant_impl_typed<ASE_VariantImplLT> {

  static const char *const debugid;

  static ase_variant invoke_self(const ase_variant& self,
    const ase_variant *args, ase_size_type nargs) {
    ase_int r = 0;
    for (ase_size_type i = 0; i < nargs; ++i) {
      r += args[i].get_int();
    }
    return ase_variant::create_int(r);
  }
  static inline ase_int GetMethodId(const ase_variant& self, const char *name,
    ase_size_type namelen);
  static inline ase_variant invoke_by_id(const ase_variant& self,
    const ase_int id, const ase_variant *args, ase_size_type nargs);
};

ase_int
ASE_VariantImplLT::GetMethodId(const ase_variant& self, const char *name,
  ase_size_type namelen)
{
  ase_size_type num = sizeof(impllt_methods) / sizeof(impllt_methods[0]);
  for (ase_size_type i = 0; i < num; ++i) {
    ase_size_type l = impllt_methods[i].u8name_len;
    if (l == namelen && std::memcmp(name, impllt_methods[i].u8name, l) == 0) {
      return static_cast<ase_int>(i);
    }
  }
  return -1;
}

ase_variant
ASE_VariantImplLT::invoke_by_id(const ase_variant& self, ase_int id,
  const ase_variant *args, ase_size_type nargs)
{
  switch (id) {
  case 0:
    {
      ase_int r = 0;
      for (ase_size_type i = 0; i < nargs; ++i) {
	r += args[i].get_int();
      }
      // fprintf(stdout, "id0 r=%d\n", r);
      return ase_variant::create_int(r);
    }
  case 1:  
    {
      double r = 0;
      for (ase_size_type i = 0; i < nargs; ++i) {
	r += args[i].get_double();
      }
      return ase_variant::create_double(r);
    }
  case 2:  
    {
      ase_int r = 0;
      if (nargs >= 1) {
	r = std::abs(args[0].get_int());
      }
      return ase_variant::create_int(r);
    }
  case 3:
    {
      if (nargs < 3) {
	ase_throw_missing_arg("libtest: missing arg");
      }
      const ase_int n = args[2].get_int();
      ase_variant r;
      for (ase_int i = 0; i < n; ++i) {
	r = args[0].invoke_self(&args[1], 1);
      }
      return r;
    }
  default:
    break;
  }
  return ase_variant();
}

const char *const
ASE_VariantImplLT::debugid = "LibTestObject";

struct ASE_TestService : public ASE_VariantFactory {
  void destroy() {
    delete this;
  }
  ase_variant NewVariant(ase_unified_engine_ref& ue,
    const ase_variant& globals_weak) {
    ase_variant v = ase_variant::create_object(
      &ase_variant_impl<ASE_VariantImplLT>::vtable,
      new ASE_VariantImplLT,
      "LibTestObject");
    /* must support GetWeak() */
    return ASE_NewStrongReference(v);
  }
};

}; // anonymous namespace

extern "C" void
ASE_NewVariantFactory_LibTest(ASE_VariantFactoryPtr& ap_r)
{
  ap_r.reset(new ASE_TestService());
}

