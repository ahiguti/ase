
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEDELAYVAR_HPP
#define GENTYPE_ASEDELAYVAR_HPP

#include <ase/aseruntime.hpp>

/* TODO: use interface instead */
typedef ase_variant (*ase_delay_load_variant_callback)(const ase_string& name,
  void *userdata);

ASELIB_DLLEXT ase_variant ase_new_delay_load_variant(
  ase_unified_engine_ref& ue, ase_delay_load_variant_callback cb,
  const ase_string& name, void *userdata);

#endif

