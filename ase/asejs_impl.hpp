
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEJS_IMPL_HPP
#define GENTYPE_ASEJS_IMPL_HPP

#include <ase/aseruntime.hpp>
#include <ase/aseatomic.hpp>
#include <ase/asemutex.hpp>
#include <ase/aselist.hpp>
#include <ase/asedllhandle.hpp>

#include <stdexcept>
#include <memory>
#include <vector>
#include <map>
#include <cstring>
#include <iostream>

#ifndef __GNUC__
#define XP_WIN
#define JS_THREADSAFE
#endif

#include <jsapi.h>
#include <jsfun.h>
#include <pmcjs.h>

namespace asejs {

extern ase_atomic_count ase_js_variant_impl_count_constr;
extern ase_atomic_count ase_js_variant_impl_count_destr;
extern ase_atomic_count ase_js_variant_impl_count_addroot;
extern ase_atomic_count ase_js_variant_impl_count_remroot;
extern ase_atomic_count ase_js_asepr_count_constr;
extern ase_atomic_count ase_js_asepr_count_destr;

struct ase_js_engine_factory : public ase_script_engine_factory {

  ase_js_engine_factory();
  ~ase_js_engine_factory();
  void destroy() {
    delete this;
  }
  void new_script_engine(ase_unified_engine_ref& ue, ase_script_engine_ptr& ap_r);

  PMC_APIFunctionTable_2 *getPMCTable();

 private:

  ase_fast_mutex asejs_mutex;
  std::auto_ptr<ase_auto_dll_handle> pmcdll;
  PMC_APIFunctionTable_2 *pmctable;
  bool pmctable_init;

};

struct ase_js_script_engine;
struct ase_js_ase_variant_impl;
struct ase_js_ase_variant_funnew_impl;
struct ase_js_aseproxy_priv;

typedef ase_list_node<ase_js_aseproxy_priv> ase_js_aseproxy_privnode;
typedef ase_list_node<ase_js_ase_variant_impl> ase_js_ase_variant_implnode;

}; // namespace asejs

#endif

