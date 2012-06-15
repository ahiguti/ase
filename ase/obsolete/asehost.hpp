
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEHOST_HPP
#define GENTYPE_ASEHOST_HPP

#include <ase/aseconfig.hpp>

struct ASE_HostService : public ase_script_engine_factory {

  ASELIB_DLLEXT ASE_HostService();
  ASELIB_DLLEXT ~ASE_HostService(); /* DCHK */
  ASELIB_DLLEXT void destroy();
  ASELIB_DLLEXT void new_script_engine(ase_unified_engine_ref& ue,
    ase_script_engine_ptr& ap_r);
  ASELIB_DLLEXT void RegisterLibrary(const ase_string& name,
    ASE_NewVariantFactoryFunc *func);

 private:

  struct impl_type;
  impl_type *impl;

};

#endif

