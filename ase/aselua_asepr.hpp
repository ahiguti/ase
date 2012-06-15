
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASELUA_ASEPR_HPP
#define GENTYPE_ASELUA_ASEPR_HPP

#include "aselua_sctx.hpp"

namespace aselua {

struct ase_lua_aseproxy_priv : public ase_variant {

 public:

  friend struct ase_lua_script_engine;

  struct init_arg {
    init_arg(ase_lua_script_engine *sc, const ase_variant& v)
      : script_engine(sc), valueref(v) { }
    ase_lua_script_engine *const script_engine;
    const ase_variant& valueref;
  };

  ase_lua_aseproxy_priv(const init_arg& ini);
  ~ase_lua_aseproxy_priv(); /* DCHK */

  ase_lua_script_engine *get_script_engine() const {
    return script_engine;
  }

  void dispose(const ase_lua_script_engine *sc);

 public:

  static int aseproxy_finalize(lua_State *luactx);
  static int aseproxy_getdouble(lua_State *luactx);
  static int aseproxy_getstring(lua_State *luactx);
  static int aseproxy_getprop(lua_State *luactx);
  static int aseproxy_setprop(lua_State *luactx);
  static int aseproxy_invoke(lua_State *luactx);
  static int aseproxy_invokebyid(lua_State *luactx);
  static int aseproxy_invokebyid_unbound(lua_State *luactx);
  static int aseproxy_invokebyname(lua_State *luactx);
  static int aseproxy_newenum(lua_State *luactx);
  static int aseproxy_enumnext(lua_State *luactx);
  static int aseproxy_eq(lua_State *luactx);

  static int aseproxy_create_unbound(lua_State *luactx);
  static int aseproxy_import_klass(lua_State *luactx);

 private:

  ase_lua_script_engine *const script_engine;

 private:

  /* noncopyable */
  ase_lua_aseproxy_priv(const ase_lua_aseproxy_priv& x);
  ase_lua_aseproxy_priv& operator =(const ase_lua_aseproxy_priv& x);

};

}; // namespace aselua

#endif

