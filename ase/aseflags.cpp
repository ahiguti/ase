
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/aseflags.hpp>
#include <ase/aseio.hpp>

#include <cstdlib>
#include <cstdio>

unsigned int ase_verbose_flag = 0;

void
ase_init_verbose_flags()
{
  ase_init_flags("ASE_VERBOSE", ase_verbose_flag);
}

void
ase_init_flags(const char *var, unsigned int& flags_r)
{
  flags_r = 0;
  ase_string c = ase_getenv_mtunsafe(var);
  if (!c.empty()) {
    #if _MSC_VER >= 1400
    ::sscanf_s(c.data(), "%x", &flags_r);
    #else
    std::sscanf(c.data(), "%x", &flags_r);
    #endif
  }
}

