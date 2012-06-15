
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEDUMMY_HPP
#define GENTYPE_ASEDUMMY_HPP

#include <ase/ase.hpp>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __GNUC__
#ifdef ASEDUMMY_EXPORTS
#define ASEDUMMY_DLLEXT __declspec(dllexport)
#else
#define ASEDUMMY_DLLEXT __declspec(dllimport)
#endif
#else
#define ASEDUMMY_DLLEXT
#endif

ASEDUMMY_DLLEXT extern const ase_script_engine_factory *
ASE_ScriptEngineFactory_DUMMY;

#ifdef __cplusplus
}
#endif

#endif

