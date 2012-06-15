
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASECLR_HPP
#define GENTYPE_ASECLR_HPP

#include <ase/ase.hpp>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __GNUC__
#ifdef ASECLR_EXPORTS
#define ASECLR_DLLEXT __declspec(dllexport)
#else
#define ASECLR_DLLEXT __declspec(dllimport)
#endif
#else
#define ASECLR_DLLEXT
#endif

ASECLR_DLLEXT extern void
ASE_NewScriptEngineFactory_CLR(ase_environment& env,
  ase_script_engine_factory_ptr& ap_r);

#ifdef __cplusplus
}
#endif

#endif
