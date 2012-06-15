
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/aseexcept.hpp>

#include "aselua_util.hpp"
#include "aselua_aseimpl.hpp"
#include "aselua_asepr.hpp"

#define DBG_TOASE(x)
#define DBG_LUAPRX(x)
#define DBG_EXC(x)

namespace aselua {

ase_string
ase_lua_util::to_ase_string(lua_State *luactx, int valindex)
{
  const char *s = lua_tostring(luactx, valindex);
  size_t len = lua_strlen(luactx, valindex);
  return ase_string(s, len);
}

void
ase_lua_util::to_lua_string(lua_State *luactx, const char *str,
  ase_size_type slen)
{
  lua_pushlstring(luactx, str, slen);
}

void
ase_lua_util::to_lua_string(lua_State *luactx, const ase_string& str)
{
  lua_pushlstring(luactx, str.data(), str.size());
}

ase_variant
ase_lua_util::to_ase_variant(ase_lua_script_engine *sc, lua_State *luactx,
  int valindex)
{
  int ty = lua_type(luactx, valindex);
  DBG_TOASE(fprintf(stderr, "LUA toase type=%d\n", ty));
  switch (ty) {
  case LUA_TNIL:
    return ase_variant();
  case LUA_TBOOLEAN:
    {
      int v = lua_toboolean(luactx, valindex);
      return ase_variant::create_boolean(v != 0);
    }
  case LUA_TNUMBER:
    {
      double v = lua_tonumber(luactx, valindex);
      return ase_variant::create_double(v);
    }
  case LUA_TSTRING:
    {
      return to_ase_string(luactx, valindex).to_variant();
    }
  case LUA_TUSERDATA:
    {
      if (lua_getmetatable(luactx, valindex)) {
	const void *ptr = lua_topointer(luactx, -1);
	lua_pop(luactx, 1); /* pop mtbl */
	if (ptr == sc->get_asepr_metatable()) {
	  /* this is an ase proxy */
	  ase_lua_aseproxy_priv *p = static_cast<ase_lua_aseproxy_priv *>(
	    lua_touserdata(luactx, valindex));
	  return *p;
	}
      }
    }
    break;
  default:
    break;
  }
  /* create a lua proxy */
  return create_luaproxy(sc, luactx, valindex);
}

void
ase_lua_util::to_lua_value(ase_lua_script_engine *sc, lua_State *luactx,
  const ase_variant& value)
{
  if (is_lua_proxy(sc, value)) {
    ase_lua_ase_variant_implnode *np =
      static_cast<ase_lua_ase_variant_implnode *>(value.get_rep().p);
    ase_lua_ase_variant_impl *p = &np->value;
    lua_pushlightuserdata(luactx, p);
    lua_rawget(luactx, LUA_REGISTRYINDEX);
    return;
  }
  ase_vtype vt = value.get_type();
  if (ase_variant::is_value_type(vt)) {
    switch (vt) {
    case ase_vtype_bool:
      {
	bool v = value.get_boolean();
	lua_pushboolean(luactx, v);
	return;
      }
    case ase_vtype_int:
    case ase_vtype_long:
    case ase_vtype_double:
      {
	double v = value.get_double();
	lua_pushnumber(luactx, v);
	return;
      }
    case ase_vtype_string:
      {
	to_lua_string(luactx, value.get_string());
	return;
      }
    default:
      lua_pushnil(luactx);
    }
    return;
  }
  return create_aseproxy(sc, luactx, value);
}

ase_variant
ase_lua_util::create_luaproxy(ase_lua_script_engine *sc, lua_State *luactx,
  int valindex)
{
  DBG_LUAPRX(fprintf(stderr, "create_luaproxy: idx=%d type=%d\n",
    valindex, lua_type(luactx, valindex)));
  ase_lua_ase_variant_implnode *p = new ase_lua_ase_variant_implnode(
    ase_lua_ase_variant_impl::init_arg(sc, luactx, valindex),
    *sc->get_lua_endnode());
  ase_variant rv = ase_variant::create_object(sc->get_variant_vtable(), p,
    "LuaObject");
  return rv;
}

void
ase_lua_util::create_aseproxy(ase_lua_script_engine *sc, lua_State *luactx,
  const ase_variant& value)
{
  void *const metatable_key = sc;
  /* create userdata */
  ase_lua_aseproxy_privnode *p = static_cast<ase_lua_aseproxy_privnode *>(
    lua_newuserdata(luactx, sizeof(ase_lua_aseproxy_privnode)));
  /* get metatable for aseproxy */
  lua_pushlightuserdata(luactx, metatable_key);
  lua_rawget(luactx, LUA_REGISTRYINDEX);
  /* set metatable */
  lua_setmetatable(luactx, -2);
  new(p) ase_lua_aseproxy_privnode(ase_lua_aseproxy_priv::init_arg(sc, value),
    *sc->get_ase_endnode());
    /* must not throw, or the finalizer will destroy uninitialized
      ase_lua_aseproxy_priv object. */
}

void
ase_lua_util::dump_gettop(lua_State *luactx, const char *mess)
{
  fprintf(stderr, "LUATOP: %s: %d\n", mess, lua_gettop(luactx));
}

#define ASE_LUA_OOM_STRING "OutOfMemory"

void
ase_lua_util::handle_ase_exception(ase_lua_script_engine *sc,
  lua_State *luactx)
{
  try {
    ase_variant ex = ase_variant::create_from_active_exception();
    DBG_EXC(sc->dump_contexts());
    luaL_where(luactx, 1);
    ase_string wh = to_ase_string(luactx, -1) + "ASEException";
    lua_pop(luactx, 1);
    ase_exception_append_trace(ex, wh.to_variant());
    to_lua_value(sc, luactx, ex);
    lua_error(luactx);
  } catch (const std::bad_alloc&) {
    const ase_variant ex = ase_new_out_of_memory_exception();
    to_lua_value(sc, luactx, ex); /* does not throw an c++ exception */
    lua_error(luactx);
  }
}

}; // namespace aselua

