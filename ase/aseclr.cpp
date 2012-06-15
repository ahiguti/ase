
#include "aseclr_util.hpp"
#include "aseclr_global.hpp"
#include "aseclr.hpp"

#include <ase/ase.hpp>

namespace aseclr {

struct ase_clr_engine_factory : public ase_script_engine_factory {

  ase_clr_engine_factory();
  void destroy() { delete this; }
  void new_script_engine(ase_unified_engine_ref& ue,
    ase_script_engine_ptr& ap_r);

};

ase_clr_engine_factory::ase_clr_engine_factory()
{
}

void
ase_clr_engine_factory::new_script_engine(ase_unified_engine_ref& ue,
  ase_script_engine_ptr& ap_r)
{
  ase_variant gl = ase_clr_create_global();
  ap_r.reset(new ase_clr_script_engine(ue, gl));
}

}; // namespace aseclr

extern "C" void
ASE_NewScriptEngineFactory_CLR(ase_environment& env,
  ase_script_engine_factory_ptr& ap_r)
{
  aseclr::ase_clr_initialize();
  ap_r.reset(new aseclr::ase_clr_engine_factory());
}

