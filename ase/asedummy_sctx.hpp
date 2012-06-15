
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEDUMMY_SCTX_HPP
#define GENTYPE_ASEDUMMY_SCTX_HPP

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

namespace asedummy {

struct ase_dummy_script_engine;
struct ase_dummy_ase_variant_impl;
struct ase_dummy_ase_variant_funnew_impl;
struct ase_dummy_aseproxy_priv;

typedef ase_list_node<ase_dummy_aseproxy_priv> ase_dummy_aseproxy_privnode;
typedef ase_list_node<ase_dummy_ase_variant_impl> ase_dummy_ase_variant_implnode;

extern ase_atomic_count ase_dummy_variant_impl_count_constr;
extern ase_atomic_count ase_dummy_variant_impl_count_destr;
extern ase_atomic_count ase_dummy_asepr_count_constr;
extern ase_atomic_count ase_dummy_asepr_count_destr;

struct ase_dummy_script_engine : public ase_script_engine {

 public:

  struct callfromase_guard : public ase_noncopyable {
    callfromase_guard(ase_dummy_script_engine *sc) : script_engine(sc) {
      script_engine->uemutex.unlock();
    }
    ~callfromase_guard() {
      script_engine->uemutex.lock();
    }
   private:
    ase_dummy_script_engine *script_engine;
  };

  struct callfromdummy_guard : public ase_noncopyable {
    callfromdummy_guard(ase_dummy_script_engine *sc) : script_engine(sc) {
      script_engine->uemutex.unlock();
    }
    ~callfromdummy_guard() {
      script_engine->uemutex.lock();
    }
   private:
    ase_dummy_script_engine *script_engine;
  };

 public:

  ase_dummy_script_engine(ase_unified_engine_ref& ue);
  ~ase_dummy_script_engine();
  void init();
  void cleanup();
  void destroy();

  void wait_threads();
  void child_thread_initialized();
  void child_thread_terminated();
  void force_gc();
  void dispose_proxies();
  const char *const *get_global_names();
  ase_variant get_global_object(const char *name);
  void import_ase_global(const ase_variant& val);
  ase_variant load_file(const char *glname, const char *filename);
  ase_script_engine *get_real_engine();

  ase_dummy_ase_variant_implnode *get_dummy_endnode() const {
    return dummy_endnode; }
  ase_dummy_aseproxy_privnode *get_ase_endnode() const { return ase_endnode; }
  ase_recursive_mutex& get_uemutex() const { return uemutex; }
  const ase_variant_vtable *get_variant_vtable() const {
    return &variant_vtable; }

 private:

  ase_unified_engine_ref& uengine;
  ase_recursive_mutex& uemutex;
  ase_variant_vtable variant_vtable;
  ase_atomic_count refcount;
  ase_dummy_ase_variant_implnode *dummy_endnode; /* dummy proxy list */
  ase_dummy_aseproxy_privnode *ase_endnode;    /* ase proxy list */
  ase_variant dummy_global;

};

}; // namespace asedummy

#endif

