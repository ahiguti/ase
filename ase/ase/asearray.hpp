
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEARRAY_HPP
#define GENTYPE_ASEARRAY_HPP

#include <ase/ase.hpp>

ASELIB_DLLEXT ase_variant ase_new_array(ase_size_type len = 0);
ASELIB_DLLEXT ase_variant ase_new_array(const ase_variant *vs,
  ase_size_type len);
ASELIB_DLLEXT ase_variant ase_array_unmodifiable(const ase_variant& arr);
  /* arr must be an object created by ase_new_array */

#endif

