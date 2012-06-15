
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asecpnt.hpp>
#include <ase/aseexcept.hpp>

#include <Rpc.h>

namespace {

struct utiluuid {

  utiluuid(const ase_string& n, ASE_Component::Library& lib) {
    std::memset(&value, 0, sizeof(value));
  }
  utiluuid(const utiluuid& klass, const ase_variant *args,
    ase_size_type nargs) {
    RPC_STATUS r = UuidCreate(&value);
    if (r != RPC_S_OK) {
      ase_throw_runtime_exception("ASEUtilUUID: failed to generate uuid");
      std::memset(&value, 0, sizeof(value));
    }
  }
  ase_variant noop(const ase_variant *args, ase_size_type nargs) {
    return ase_variant();
  }
  ase_variant gen(const ase_variant *args, ase_size_type nargs) {
    unsigned char *str = 0;
    RPC_STATUS r = UuidToString(&value, &str);
    ase_variant rv;
    if (r == RPC_S_OK) {
      char *s = reinterpret_cast<char *>(str);
      ase_string as(s);
      rv = as.to_variant();
    }
    RpcStringFree(&str);
    if (r == RPC_S_OUT_OF_MEMORY) {
      throw std::bad_alloc();
    } else if (r != RPC_S_OK) {
      ase_throw_runtime_exception("ASEUtilUUID: failed to get uuid");
    }
    return ase_variant();
  }

  uuid_t value;

  static const ASE_Component::MethodEntry<utiluuid> ClassMethods[];
  static const ASE_Component::MethodEntry<utiluuid> InstanceMethods[];

};

const ASE_Component::MethodEntry<utiluuid> utiluuid::ClassMethods[] = {
  { "noop", &utiluuid::noop },
};

const ASE_Component::MethodEntry<utiluuid> utiluuid::InstanceMethods[] = {
  { "upper", &utiluuid::gen },
  { "lower", &utiluuid::gen },
};

}; // anonymous namespace

ase_variant
ASE_NewUtilUUIDClass(const ase_string& n, ASE_Component::Library& lib)
{
  return ASE_Component::NewClass<utiluuid, utiluuid, true>(n, lib);
}

