
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEJS_HPP
#define GENTYPE_ASEJS_HPP

#include <ase/ase.hpp>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __GNUC__
#ifdef ASEJS_EXPORTS
#define ASEJS_DLLEXT __declspec(dllexport)
#else
#define ASEJS_DLLEXT __declspec(dllimport)
#endif
#else
#define ASEJS_DLLEXT
#endif

ASEJS_DLLEXT void
ASE_NewScriptEngineFactory_JS(ase_environment& env,
  ase_script_engine_factory_ptr& ap_r);

#ifdef __cplusplus
}
#endif

#endif

