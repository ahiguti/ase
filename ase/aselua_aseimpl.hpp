
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASELUA_ASEIMPL_HPP
#define GENTYPE_ASELUA_ASEIMPL_HPP

#include "aselua_sctx.hpp"

namespace aselua {

struct ase_lua_ase_variant_impl : public ase_noncopyable {

 public:

  friend struct ase_lua_script_engine;
  friend struct ase_lua_ase_variant_glfunc_impl;

  static ase_lua_ase_variant_implnode *get(const ase_variant& self) {
    return static_cast<ase_lua_ase_variant_implnode *>(self.get_rep().p);
  }
  static ase_lua_script_engine *getctx(const ase_variant& self) {
    return static_cast<ase_lua_script_engine *>(self.get_impl_priv());
  }
  static ase_lua_ase_variant_impl *get_regkey(
    ase_lua_ase_variant_implnode *selfp) {
    return &selfp->value;
  }

  struct init_arg {
    init_arg(ase_lua_script_engine *sc, lua_State *cx, int vidx)
      : script_engine(sc), luactx(cx), valindex(vidx) { }	
    ase_lua_script_engine *const script_engine;
    lua_State *const luactx;
    const int valindex;
  };

  ase_lua_ase_variant_impl(const init_arg& arg);
  ~ase_lua_ase_variant_impl(); /* DCHK */
  static void add_ref(const ase_variant& self);
  static void release(const ase_variant& self);
  static ase_int get_attributes(const ase_variant& self);
  static bool get_boolean(const ase_variant& self);
  static ase_int get_int(const ase_variant& self);
  static ase_long get_long(const ase_variant& self);
  static double get_double(const ase_variant& self);
  static ase_string get_string(const ase_variant& self);
  static bool is_same_object(const ase_variant& self, const ase_variant& value);
  static ase_variant get_property(const ase_variant& self,
    const char *name, ase_size_type namelen);
  static void set_property(const ase_variant& self, const char *name,
    ase_size_type namelen, const ase_variant& value);
  static void del_property(const ase_variant& self, const char *name,
    ase_size_type namelen);
  static ase_variant get_element(const ase_variant& self, ase_int idx);
  static void set_element(const ase_variant& self, ase_int idx,
    const ase_variant& value);
  static ase_int get_length(const ase_variant& self);
  static void set_length(const ase_variant& self, ase_int len);
  static ase_variant invoke_by_name(const ase_variant& self,
    const char *name, ase_size_type namelen, const ase_variant *args,
    ase_size_type nargs);
  static ase_variant invoke_by_id(const ase_variant& self, ase_int id,
    const ase_variant *args, ase_size_type nargs);
  static ase_variant invoke_self(const ase_variant& self,
    const ase_variant *args, ase_size_type nargs);
  static ase_variant get_enumerator(const ase_variant& self);
  static ase_string enum_next(const ase_variant& self, bool& hasnext_r);

 public:

  void dispose(ase_lua_script_engine *sc, lua_State *luactx);

 private:

  mutable ase_atomic_count refcount;

};

}; // namespace aselua

#endif

