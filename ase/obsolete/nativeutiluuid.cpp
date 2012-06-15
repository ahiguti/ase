
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asecpnt.hpp>

#include <uuid/uuid.h>

namespace {

struct utiluuid {

  utiluuid(const ase_string& n, ASE_Component::Library& lib) { }
  utiluuid(const utiluuid& klass, const ase_variant *args,
    ase_size_type nargs) {
    uuid_generate(value);
  }
  ase_variant upper(const ase_variant *args, ase_size_type nargs) {
    char buf[37];
    uuid_unparse_upper(value, buf);
    return ase_variant(buf, 36);
  }
  ase_variant lower(const ase_variant *args, ase_size_type nargs) {
    char buf[37];
    uuid_unparse_lower(value, buf);
    return ase_variant(buf, 36);
  }

  uuid_t value;

  static const ASE_Component::MethodEntry<utiluuid> ClassMethods[];
  static const ASE_Component::MethodEntry<utiluuid> InstanceMethods[];

};

const ASE_Component::MethodEntry<utiluuid> utiluuid::ClassMethods[] = {
};

const ASE_Component::MethodEntry<utiluuid> utiluuid::InstanceMethods[] = {
  { "upper", &utiluuid::upper },
  { "lower", &utiluuid::lower },
};

}; // anonymous namespace

ase_variant
ASE_NewUtilUUIDClass(const ase_string& name, ASE_Component::Library& lib)
{
  return ASE_Component::NewClass<
    utiluuid, utiluuid, true
  >(name, lib);
}

