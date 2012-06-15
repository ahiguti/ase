
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASELUA_UTIL_HPP
#define GENTYPE_ASELUA_UTIL_HPP

#include "aselua_sctx.hpp"

namespace aselua {

struct ase_lua_util {

  static inline bool is_lua_proxy(ase_lua_script_engine *sc,
    const ase_variant& value) {
    return value.get_impl_priv() == sc;
  }

  static ase_variant create_luaproxy(ase_lua_script_engine *sc,
    lua_State *luactx, int valindex);
  static void create_aseproxy(ase_lua_script_engine *sc, lua_State *luactx,
    const ase_variant& value);

  static ase_string to_ase_string(lua_State *luactx, int valindex);
  static void to_lua_string(lua_State *luactx, const char *str,
    ase_size_type slen);
  static void to_lua_string(lua_State *luactx, const ase_string& str);
  static ase_variant to_ase_variant(ase_lua_script_engine *sc,
    lua_State *luactx, int valindex);
  static void to_lua_value(ase_lua_script_engine *sc, lua_State *luactx,
    const ase_variant& value);
  static void dump_gettop(lua_State *luactx, const char *mess);

  static void handle_ase_exception(ase_lua_script_engine *sc,
    lua_State *luactx);

};

}; // namespace aselua

#endif

