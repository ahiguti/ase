
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEJNI_HPP
#define GENTYPE_ASEJNI_HPP

#include <ase/ase.hpp>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __GNUC__
#ifdef ASEJNI_EXPORTS
#define ASEJNI_DLLEXT __declspec(dllexport)
#else
#define ASEJNI_DLLEXT __declspec(dllimport)
#endif
#else
#define ASEJNI_DLLEXT
#endif

ASEJNI_DLLEXT void
ASE_NewScriptEngineFactory_Java(ase_environment& env,
  ase_script_engine_factory_ptr& ap_r);

#ifdef __cplusplus
}
#endif

#endif

