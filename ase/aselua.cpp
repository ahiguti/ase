
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include "aselua_sctx.hpp"
#include "aselua.hpp"

namespace aselua {

ase_atomic_count ase_lua_variant_impl_count_constr = 0;
ase_atomic_count ase_lua_variant_impl_count_destr = 0;
ase_atomic_count ase_lua_asepr_count_constr = 0;
ase_atomic_count ase_lua_asepr_count_destr = 0;

struct ase_lua_engine_factory : public ase_script_engine_factory {

  ~ase_lua_engine_factory(); /* DCHK */
  void destroy() { delete this; }
  void new_script_engine(ase_unified_engine_ref& ue,
    ase_script_engine_ptr& ap_r);

};

ase_lua_engine_factory::~ase_lua_engine_factory() /* DCHK */
{
  ASE_DEBUG(fprintf(stderr, "LUA LUAPR CONSTR %d\n", 
    (int)ase_lua_variant_impl_count_constr));
  ASE_DEBUG(fprintf(stderr, "LUA LUAPR DESTR  %d\n", 
    (int)ase_lua_variant_impl_count_destr));
  ASE_DEBUG(fprintf(stderr, "LUA ASEPR CONSTR %d\n", 
    (int)ase_lua_asepr_count_constr));
  ASE_DEBUG(fprintf(stderr, "LUA ASEPR DESTR  %d\n", 
    (int)ase_lua_asepr_count_destr));
}

void
ase_lua_engine_factory::new_script_engine(ase_unified_engine_ref& ue,
  ase_script_engine_ptr& ap_r)
{
  ap_r.reset(new ase_lua_script_engine(ue));
}

}; // namespace aselua


extern "C" void
ASE_NewScriptEngineFactory_Lua(ase_environment& env,
  ase_script_engine_factory_ptr& ap_r)
{
  ap_r.reset(new aselua::ase_lua_engine_factory());
}

