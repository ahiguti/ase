
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASECLR_SCTX_HPP
#define GENTYPE_ASECLR_SCTX_HPP

#include <vcclr.h>
#include <ase/aseruntime.hpp>
#include <ase/aseatomic.hpp>

#include "aseclr_clrpr.hpp"
#include "aseclr_dynproxy.hpp"

namespace aseclr {

struct ase_clr_script_engine : public ase_script_engine {

  ase_clr_script_engine(ase_unified_engine_ref& ue, const ase_variant& gl);
  ~ase_clr_script_engine();

  void destroy() {
    delete this;
  }
  void child_thread_initialized() { }
  void child_thread_terminated() { }
  void force_gc() { }
  void wait_threads() { }
  void dispose_proxies();
  const char *const *get_global_names();
  ase_variant get_global_object(const char *name);
  void import_ase_global(const ase_variant& val);
  ase_variant load_file(const char *glname, const char *fname);
  ase_script_engine *get_real_engine();
  
  ase_clr_ase_variant_implnode *get_clrpr_endnode() {
    return clrpr_endnode;
  }
  aseproxy_node *get_asepr_endnode() {
    return asepr_endnode;
  }

 private:

  ase_unified_engine_ref& ueref;
  ase_clr_ase_variant_implnode *clrpr_endnode; /* clr proxy list */
  aseproxy_node *asepr_endnode; /* ase proxy list */
  ase_variant global;

};

}; // namespace aseclr

#endif

