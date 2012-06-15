
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEBINDMETHOD_HPP
#define GENTYPE_ASEBINDMETHOD_HPP

#include <ase/ase.hpp>

ASELIB_DLLEXT ase_variant ase_new_variant_bind_method_id(
  const ase_variant& val, ase_int id);
ASELIB_DLLEXT ase_variant ase_new_variant_bind_method_name(
  const ase_variant& val, const ase_string& s);

#endif

