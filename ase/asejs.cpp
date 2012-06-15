
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asemutex.hpp>
#include <ase/aseflags.hpp>
#include <ase/aseio.hpp>
#include <ase/asemalloc.hpp>

#include "asejs_sctx.hpp"
#include "asejs_util.hpp"
#include "asejs.hpp"

namespace asejs {

ase_atomic_count ase_js_variant_impl_count_constr = 0;
ase_atomic_count ase_js_variant_impl_count_destr = 0;
ase_atomic_count ase_js_variant_impl_count_addroot = 0;
ase_atomic_count ase_js_variant_impl_count_remroot = 0;
ase_atomic_count ase_js_asepr_count_constr = 0;
ase_atomic_count ase_js_asepr_count_destr = 0;

ase_js_engine_factory::ase_js_engine_factory()
  : pmctable(0), pmctable_init(false)
{
  ase_init_flags("ASE_JS_VERBOSE", asejs::ase_js_util::verbose);
}

PMC_APIFunctionTable_2 *
ase_js_engine_factory::getPMCTable()
{
  ase_mutex_guard<ase_fast_mutex> g(asejs_mutex);
  if (pmctable_init) {
    return pmctable;
  }
  pmcdll.reset(new ase_auto_dll_handle(ase_string("libpmcjs" ASE_DLL_SUFFIX),
    true));
  if (pmcdll->get_error().empty()) {
    ASE_JS_VERBOSE_PMC(fprintf(stderr, "PMC OPEN\n"));
    typedef PMC_APIFunctionTable_2 *getpmcapi_type();
    getpmcapi_type *getpmcapi = (getpmcapi_type *)(pmcdll->get_symbol(
      ase_string("PMC_GetAPIFunctionTable_2")));
    if (getpmcapi) {
      pmctable = getpmcapi();
      if (!pmctable->PMC_PerlAvailable()) {
        ASE_JS_VERBOSE_PMC(fprintf(stderr, "PMC: perl dll NOTFOUND\n"));
        pmctable = 0;
        pmcdll.reset();
      }
    } else {
      ASE_JS_VERBOSE_PMC(fprintf(stderr, "PMC: api entry NOTFOUND\n"));
      pmcdll.reset();
    }
  } else {
    ASE_JS_VERBOSE_PMC(fprintf(stderr, "PMC: libpmcjs NOTFOUND\n"));
  }
  if (pmctable) {
    /* pmcjs may leak when the perl interpreter gets oom and longjumps. 
      to avoid this, we call _exit() immediately when malloc is failed. */
    /* note: this is virtually useless for win32 because activeperl simply
      crashes in case of oom */
    ase_install_malloc_hook();

    char *arr[] = { 0 };
    pmctable->PMC_Init(0, arr, 0);
  }
  pmctable_init = true;
  return pmctable;
}

ase_js_engine_factory::~ase_js_engine_factory() /* DCHK */
{
  /* should be constr >= addroot == remroot, constr >= destr */
  ASE_DEBUG(fprintf(stderr, "JS JSPR CONSTR %d\n", 
    (int)ase_js_variant_impl_count_constr));
  ASE_DEBUG(fprintf(stderr, "JS JSPR DESTR  %d\n", 
    (int)ase_js_variant_impl_count_destr));
  ASE_DEBUG(fprintf(stderr, "JS JSPR ADDROOT %d\n", 
    (int)ase_js_variant_impl_count_addroot));
  ASE_DEBUG(fprintf(stderr, "JS JSPR REMROOT %d\n", 
    (int)ase_js_variant_impl_count_remroot));
  ASE_DEBUG(fprintf(stderr, "JS ASEPR CONSTR %d\n", 
    (int)ase_js_asepr_count_constr));
  ASE_DEBUG(fprintf(stderr, "JS ASEPR DESTR  %d\n", 
    (int)ase_js_asepr_count_destr));
  if (pmctable) {
    pmctable->PMC_Terminate();
  }
  JS_ShutDown();
}

void
ase_js_engine_factory::new_script_engine(ase_unified_engine_ref& ue,
  ase_script_engine_ptr& ap_r)
{
  ap_r.reset(new ase_js_script_engine(ue, this));
}

}; // namespace asejs

extern "C" void
ASE_NewScriptEngineFactory_JS(ase_environment& env,
  ase_script_engine_factory_ptr& ap_r)
{
  ap_r.reset(new asejs::ase_js_engine_factory());
}

