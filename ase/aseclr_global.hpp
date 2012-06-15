
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASECLR_GLOBAL_HPP
#define GENTYPE_ASECLR_GLOBAL_HPP

#include "aseclr_util.hpp"

namespace aseclr {

void ase_clr_initialize();
ase_variant ase_clr_create_global();
void ase_clr_set_engine_ref(const ase_variant& globj,
  ase_clr_script_engine *sc);

}; // namespace aseclr

#endif

