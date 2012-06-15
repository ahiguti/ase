
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#define _WIN32_DCOM
#include <objbase.h>

#include <ase/aseflags.hpp>

#include "asecom_sctx.hpp"
#include "asecom_util.hpp"
#include "asecom.hpp"

#define DBG(x)

namespace asecom {

ase_atomic_count ase_com_variant_impl_count_constr = 0;
ase_atomic_count ase_com_variant_impl_count_destr = 0;
ase_atomic_count ase_com_asepr_count_constr = 0;
ase_atomic_count ase_com_asepr_count_destr = 0;

struct ase_com_engine_factory : public ase_script_engine_factory {

  ase_com_engine_factory(ase_environment& e);
  ~ase_com_engine_factory(); /* DCHK */
  void destroy() {
    delete this;
  }
  void new_script_engine(ase_unified_engine_ref& ue,
    ase_script_engine_ptr& ap_r);

  ase_environment& env;

};

ase_com_engine_factory::ase_com_engine_factory(ase_environment& e)
  : env(e)
{
  ase_init_flags("ASE_COM_VERBOSE", ase_com_util::verbose);
  DBG(fprintf(stderr, "coinit main\n"));
  ase_com_util::init_main_thread(); /* throws if failed */
}

ase_com_engine_factory::~ase_com_engine_factory() /* DCHK */
{
  ase_com_util::uninit_main_thread();
  ASE_DEBUG(fprintf(stderr, "COM COMPR CONSTR %d\n", 
    (int)ase_com_variant_impl_count_constr));
  ASE_DEBUG(fprintf(stderr, "COM COMPR DESTR  %d\n", 
    (int)ase_com_variant_impl_count_destr));
  ASE_DEBUG(fprintf(stderr, "COM ASEPR CONSTR %d\n", 
    (int)ase_com_asepr_count_constr));
  ASE_DEBUG(fprintf(stderr, "COM ASEPR DESTR  %d\n", 
    (int)ase_com_asepr_count_destr));
}

void
ase_com_engine_factory::new_script_engine(ase_unified_engine_ref& ue,
  ase_script_engine_ptr& ap_r)
{
  ap_r.reset(new ase_com_script_engine());
}

}; // namespace asecom


extern "C" void
ASE_NewScriptEngineFactory_COM(ase_environment& env,
  ase_script_engine_factory_ptr& ap_r)
{
  ap_r.reset(new asecom::ase_com_engine_factory(env));
}

