
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASELUA_SCTX_HPP
#define GENTYPE_ASELUA_SCTX_HPP

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

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

namespace aselua {

struct ase_lua_script_engine;
struct ase_lua_ase_variant_impl;
struct ase_lua_ase_variant_funnew_impl;
struct ase_lua_aseproxy_priv;

typedef ase_list_node<ase_lua_aseproxy_priv> ase_lua_aseproxy_privnode;
typedef ase_list_node<ase_lua_ase_variant_impl> ase_lua_ase_variant_implnode;

extern ase_atomic_count ase_lua_variant_impl_count_constr;
extern ase_atomic_count ase_lua_variant_impl_count_destr;
extern ase_atomic_count ase_lua_asepr_count_constr;
extern ase_atomic_count ase_lua_asepr_count_destr;

struct ase_lua_script_engine : public ase_script_engine {

  struct callfromase_guard {

    callfromase_guard(ase_lua_script_engine *sc);
    ~callfromase_guard(); /* DCHK */
    lua_State *get_current_luactx() const { return luactx; }

   private:

    ase_lua_script_engine *script_engine;
    lua_State *luactx;

   private:

    /* noncopyable */
    callfromase_guard(const callfromase_guard& x);
    callfromase_guard& operator =(const callfromase_guard& x);

  };

  struct callfromlua_guard {

    callfromlua_guard(const ase_variant& v, ase_lua_script_engine *sc);
    ~callfromlua_guard();

   private:

    bool unlock_flag;
    ase_lua_script_engine *script_engine;

   private:

    /* noncopyable */
    callfromlua_guard(const callfromlua_guard& x);
    callfromlua_guard& operator =(const callfromlua_guard& x);

  };

  typedef std::vector<lua_State *> cached_contexts_type;

 public:

  ase_lua_script_engine(ase_unified_engine_ref& ue);
  ~ase_lua_script_engine(); /* DCHK */
  void init();
  void cleanup();

  void destroy() {
    delete this;
  }
  void child_thread_initialized() { }
  void child_thread_terminated() { }
  void force_gc();
  void wait_threads() { }
  void dispose_proxies();
  const char *const *get_global_names();
  ase_variant get_global_object(const char *name);
  void import_ase_global(const ase_variant& val);
  ase_variant load_file(const char *glname, const char *filename);
  ase_script_engine *get_real_engine();

  ase_lua_ase_variant_implnode *get_lua_endnode() const {
    return lua_endnode; }
  ase_lua_aseproxy_privnode *get_ase_endnode() const { return ase_endnode; }
  ase_recursive_mutex& get_uemutex() const { return uemutex; }
  const ase_variant_vtable *get_variant_vtable() const {
    return &variant_vtable; }
  const void *get_asepr_metatable() const { return asepr_metatable; }
  
  void dump_contexts();

 private:

  ase_recursive_mutex& uemutex;
  ase_variant_vtable variant_vtable;
  lua_State *default_luactx;
  const void *asepr_metatable;
  ase_lua_ase_variant_implnode *lua_endnode; /* lua proxy list */
  ase_lua_aseproxy_privnode *ase_endnode;    /* ase proxy list */
  cached_contexts_type cached_contexts;
  ase_variant luaglobal;

};

}; // namespace aselua

#endif

