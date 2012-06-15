
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASECOM_SCTX_HPP
#define GENTYPE_ASECOM_SCTX_HPP

#include <ase/aseruntime.hpp>
#include <ase/aseatomic.hpp>
#include <ase/asemutex.hpp>
#include <ase/aselist.hpp>
#include <ase/asedllhandle.hpp>
#include <ase/aseflags.hpp>

#include <stdexcept>
#include <memory>
#include <vector>
#include <map>
#include <cstring>
#include <iostream>

namespace asecom {

struct ase_com_script_engine;
struct ase_com_ase_variant_impl;
struct ase_com_ase_variant_funnew_impl;
struct ase_com_aseproxy_priv;
struct ase_com_aseproxy;

typedef ase_list_node<ase_com_aseproxy_priv> ase_com_aseproxy_privnode;
typedef ase_list_node<ase_com_ase_variant_impl> ase_com_ase_variant_implnode;

extern ase_atomic_count ase_com_variant_impl_count_constr;
extern ase_atomic_count ase_com_variant_impl_count_destr;
extern ase_atomic_count ase_com_asepr_count_constr;
extern ase_atomic_count ase_com_asepr_count_destr;

struct ase_com_script_engine : ase_script_engine {

 public:

  ase_com_script_engine();
  ~ase_com_script_engine(); /* DCHK */
  void destroy();
  void init();
  void cleanup();

  void child_thread_initialized();
  void child_thread_terminated();
  void force_gc() { }
  void wait_threads() { }
  void dispose_proxies();
  const char *const *get_global_names();
  ase_variant get_global_object(const char *name);
  void import_ase_global(const ase_variant& val);
  ase_variant load_file(const char *glname, const char *fname);
  ase_script_engine *get_real_engine();

  ase_com_ase_variant_implnode *get_com_endnode() const {
    return com_endnode; }
  ase_com_aseproxy *get_ase_endnode() const { return ase_endnode; }
  const ase_variant_vtable *get_variant_vtable() const {
    return &variant_vtable; }
  void *get_asepr_com_vtable_addr() const {
    return asepr_com_vtable_addr; }

 private:

  ase_variant_vtable variant_vtable;
  ase_atomic_count refcount;
  ase_com_ase_variant_implnode *com_endnode; /* com proxy list */
  ase_com_aseproxy *ase_endnode;    /* ase proxy list */
  ase_variant comglobal;
  void *asepr_com_vtable_addr;

};

}; // namespace asecom

#endif

