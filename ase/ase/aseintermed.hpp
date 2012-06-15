
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEINTERMED_HPP
#define GENTYPE_ASEINTERMED_HPP

#include <ase/ase.hpp>

ASELIB_DLLEXT ase_variant ase_new_intermediate_property(const ase_variant& val,
  const ase_string& name /* must end with a delimiter */);
ASELIB_DLLEXT ase_variant ase_get_property_chained(const ase_variant& val,
  const char *name, ase_size_type namelen, char delim);

#endif

