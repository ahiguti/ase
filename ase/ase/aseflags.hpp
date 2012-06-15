
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASE_FLAGS_HPP
#define GENTYPE_ASE_FLAGS_HPP

#include <ase/ase.hpp>

ASELIB_DLLEXT extern unsigned int ase_verbose_flag;
ASELIB_DLLEXT void ase_init_verbose_flags(); /* getenv("ASE_VERBOSE") */
ASELIB_DLLEXT void ase_init_flags(const char *evar, unsigned int& flags_r);
  /* uses getenv() */

#define ASE_VERBOSE_OBJECT(x) if ((ase_verbose_flag &  1) != 0) { x; };
#define ASE_VERBOSE_STRING(x) if ((ase_verbose_flag &  2) != 0) { x; };
#define ASE_VERBOSE_DELAY(x)  if ((ase_verbose_flag &  4) != 0) { x; };
#define ASE_VERBOSE_WEAK(x)   if ((ase_verbose_flag &  8) != 0) { x; };
#define ASE_VERBOSE_RUN(x)    if ((ase_verbose_flag & 16) != 0) { x; };
#define ASE_VERBOSE_DLL(x)    if ((ase_verbose_flag & 32) != 0) { x; };
#define ASE_VERBOSE_CONF(x)   if ((ase_verbose_flag & 64) != 0) { x; };

#ifndef NDEBUG
#define ASE_DEBUG(x) x
#else
#define ASE_DEBUG(x)
#endif

#endif


