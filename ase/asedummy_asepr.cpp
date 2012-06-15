
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include "asedummy_util.hpp"
#include "asedummy_asepr.hpp"

#ifdef _MSC_VER
#include <malloc.h>
#define ASE_JS_STACK_ALLOCA(x) _alloca(x)
#else
#include <alloca.h>
#define ASE_JS_STACK_ALLOCA(x) alloca(x)
#endif


#define DBG(x)
#define DBG_ARR(x)

namespace asedummy {

ase_dummy_aseproxy_priv::ase_dummy_aseproxy_priv(const init_arg& ini)
  : ase_variant(ini.valueref), script_engine(ini.script_engine)
{
  DBG(fprintf(stderr, "ASEPROXY constr %p\n", this));
  ASE_DEBUG(ase_dummy_asepr_count_constr++);
}

ase_dummy_aseproxy_priv::~ase_dummy_aseproxy_priv() /* DCHK */
{
  DBG(fprintf(stderr, "ASEPROXY destr %p\n", this));
  ASE_DEBUG(ase_dummy_asepr_count_destr++);
}

void
ase_dummy_aseproxy_priv::dispose(const ase_dummy_script_engine *sc)
{
  ase_variant& s = *this;
  s = ase_variant();
}

}; // namespace asedummy

