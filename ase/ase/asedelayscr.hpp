
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEDELAYSCR_HPP
#define GENTYPE_ASEDELAYSCR_HPP

#include <ase/aseruntime.hpp>

ASELIB_DLLEXT void ase_new_delay_load_script_engine_factory(ase_environment& e,
  const ase_string& servicename, const ase_string& dllfilename,
  const ase_string *globalnames, ase_size_type globalnames_num,
  void *mainthr, ase_script_engine_factory_ptr& ap_r);

#endif

