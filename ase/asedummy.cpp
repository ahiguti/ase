
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include "asedummy_sctx.hpp"
#include "asedummy.hpp"

namespace asedummy {

ase_atomic_count ase_dummy_variant_impl_count_constr = 0;
ase_atomic_count ase_dummy_variant_impl_count_destr = 0;
ase_atomic_count ase_dummy_asepr_count_constr = 0;
ase_atomic_count ase_dummy_asepr_count_destr = 0;

struct ase_dummy_engine_factory : public ase_script_engine_factory {

  ase_dummy_engine_factory(ase_environment& e);
  ~ase_dummy_engine_factory(); /* DCHK */
  void destroy() { delete this; }
  void new_script_engine(ase_unified_engine_ref& ue,
    ase_script_engine_ptr& ap_r);

};

ase_dummy_engine_factory::ase_dummy_engine_factory(ase_environment& e)
{
}

ase_dummy_engine_factory::~ase_dummy_engine_factory() /* DCHK */
{
  ASE_DEBUG(fprintf(stderr, "DUMMY DUMMYPR CONSTR %d\n", 
    (int)ase_dummy_variant_impl_count_constr));
  ASE_DEBUG(fprintf(stderr, "DUMMY DUMMYPR DESTR  %d\n", 
    (int)ase_dummy_variant_impl_count_destr));
  ASE_DEBUG(fprintf(stderr, "DUMMY ASEPR CONSTR %d\n", 
    (int)ase_dummy_asepr_count_constr));
  ASE_DEBUG(fprintf(stderr, "DUMMY ASEPR DESTR  %d\n", 
    (int)ase_dummy_asepr_count_destr));
}

void
ase_dummy_engine_factory::new_script_engine(ase_unified_engine_ref& ue,
  ase_script_engine_ptr& ap_r)
{
  ap_r.reset(new ase_dummy_script_engine(ue));
}

}; // namespace asedummy


extern "C" void
ASE_NewScriptEngineFactory_Dummy(ase_environment& env,
  ase_script_engine_factory_ptr& ap_r)
{
  ap_r.reset(new asedummy::ase_dummy_engine_factory(env));
}

