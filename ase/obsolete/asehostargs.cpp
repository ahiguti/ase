
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asehostargs.hpp>
#include <ase/asearray.hpp>

#define DBG(x)

namespace {

struct ase_host_args : public ASE_VariantFactory {

  ase_host_args() {
    DBG(fprintf(stderr, "hostargs initialize\n"));
  }
  ~ase_host_args() /* DCHK */ {
    DBG(fprintf(stderr, "hostargs terminate\n"));
  }
  void destroy() {
    delete this;
  }
  ase_variant NewVariant(ase_unified_engine_ref& ue,
    const ase_variant& globals) {
    DBG(fprintf(stderr, "create args object\n"));
    ase_variant r = ase_new_array();
    DBG(fprintf(stderr, "arr %s\n", r.get_string().data()));
    return r;
  }

};

}; // namespace

void
ASE_HostArgsService(ASE_VariantFactoryPtr& ap_r)
{
  return ap_r.reset(new ase_host_args());
}

