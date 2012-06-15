
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASESTRMAP_HPP
#define GENTYPE_ASESTRMAP_HPP

#include <ase/ase.hpp>

ASELIB_DLLEXT ase_variant ase_new_string_map();
ASELIB_DLLEXT ase_variant ase_string_map_unmodifiable(const ase_variant& m);
  /* m must be an object created by ase_new_string_map */
ASELIB_DLLEXT void ase_string_map_clear(const ase_variant& m);

#endif

