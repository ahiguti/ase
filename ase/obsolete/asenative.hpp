
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASENATIVE_HPP
#define GENTYPE_ASENATIVE_HPP

#include <ase/asecpp.hpp>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __GNUC__
#ifdef ASENATIVE_EXPORTS
#define ASENATIVE_DLLEXT __declspec(dllexport)
#else
#define ASENATIVE_DLLEXT __declspec(dllimport)
#endif
#else
#define ASENATIVE_DLLEXT
#endif

ASENATIVE_DLLEXT void
ASE_NewScriptEngineFactory_Native(ase_environment& env,
  ase_script_engine_factory_ptr& ap_r);

#ifdef __cplusplus
}
#endif

#endif

