
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASECPPLODER_HPP
#define GENTYPE_ASECPPLODER_HPP

#include <ase/aseruntime.hpp>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __GNUC__
#ifdef ASECPPLOADER_EXPORTS
#define ASECPPLOADER_DLLEXT __declspec(dllexport)
#else
#define ASECPPLOADER_DLLEXT __declspec(dllimport)
#endif
#else
#define ASECPPLOADER_DLLEXT
#endif

ASECPPLOADER_DLLEXT void
ASE_NewScriptEngineFactory_CPP(ase_environment& env,
  ase_script_engine_factory_ptr& ap_r);

#ifdef __cplusplus
}
#endif

#endif

