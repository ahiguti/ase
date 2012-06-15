
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASELUA_HPP
#define GENTYPE_ASELUA_HPP

#include <ase/ase.hpp>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __GNUC__
#ifdef ASELUA_EXPORTS
#define ASELUA_DLLEXT __declspec(dllexport)
#else
#define ASELUA_DLLEXT __declspec(dllimport)
#endif
#else
#define ASELUA_DLLEXT
#endif

ASELUA_DLLEXT void
ASE_NewScriptEngineFactory_Lua(ase_environment& env,
  ase_script_engine_factory_ptr& ap_r);

#ifdef __cplusplus
}
#endif

#endif

