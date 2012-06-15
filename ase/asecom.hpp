
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASECOM_HPP
#define GENTYPE_ASECOM_HPP

#include <ase/ase.hpp>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __GNUC__
#ifdef ASECOM_EXPORTS
#define ASECOM_DLLEXT __declspec(dllexport)
#else
#define ASECOM_DLLEXT __declspec(dllimport)
#endif
#else
#define ASECOM_DLLEXT
#endif

ASECOM_DLLEXT extern void
ASE_NewScriptEngineFactory_COM(ase_environment& env,
  ase_script_engine_factory_ptr& ap_r);

#ifdef __cplusplus
}
#endif

#endif

