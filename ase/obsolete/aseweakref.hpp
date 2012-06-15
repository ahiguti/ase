
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEWEAK_HPP
#define GENTYPE_ASEWEAK_HPP

#include <ase/asecpp.hpp>

ASELIB_DLLEXT ase_variant ASE_NewStrongReference(const ase_variant& value);
ASELIB_DLLEXT ase_variant ASE_ExtractReference(const ase_variant& value);

#endif


