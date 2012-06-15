
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/aseexcept.hpp>

#include "aselua_util.hpp"
#include "aselua_asepr.hpp"

#include <cassert>

#ifdef _MSC_VER
#include <malloc.h>
#define ASE_JS_STACK_ALLOCA(x) _alloca(x)
#else
#include <alloca.h>
#define ASE_JS_STACK_ALLOCA(x) alloca(x)
#endif


#define DBG(x)
#define DBG_FIN(x)
#define DBG_ARR(x)
#define DBG_UNBOUND(x)
#define DBG_IMPORT(x)

namespace aselua {

ase_lua_aseproxy_priv::ase_lua_aseproxy_priv(const init_arg& ini)
  : ase_variant(ini.valueref), script_engine(ini.script_engine)
{
  DBG(fprintf(stderr, "ASEPROXY constr %p\n", this));
  ASE_DEBUG(ase_lua_asepr_count_constr++);
}

ase_lua_aseproxy_priv::~ase_lua_aseproxy_priv() /* DCHK */
{
  DBG(fprintf(stderr, "ASEPROXY destr %p\n", this));
  ASE_DEBUG(ase_lua_asepr_count_destr++);
}

void
ase_lua_aseproxy_priv::dispose(const ase_lua_script_engine *sc)
{
  DBG(fprintf(stderr, "ASEPROXY dispose %p\n", this));
  ase_variant& s = *this;
  s = ase_variant();
}

int
ase_lua_aseproxy_priv::aseproxy_finalize(lua_State *luactx)
{
  ase_lua_aseproxy_privnode *p = static_cast<ase_lua_aseproxy_privnode *>(
    lua_touserdata(luactx, 1));
  DBG_FIN(fprintf(stderr, "ASEPROXY finalize %p\n", p));
  if (!p) {
    return 0;
  }
  p->~ase_lua_aseproxy_privnode(); /* DCHK */
  return 0;
}

namespace {

struct ase_lua_asearrref {
  ase_lua_asearrref() : arr(0), num(0) { }
  void set(ase_variant *a, int n, int firstindex, ase_lua_script_engine *sc,
    lua_State *luactx) __attribute__((nonnull (1))) {
    assert(arr == 0);
    arr = a;
    for (int i = 0; i < n; ++i) {
      new(a + i) ase_variant(ase_lua_util::to_ase_variant(sc, luactx,
	firstindex + i));
      DBG_ARR(fprintf(stderr, "asearrref constr %u %p\n", i,
        a[i].get_rep().p));
      num = i + 1;
    }
  }
  ~ase_lua_asearrref() /* DCHK */ {
    for (int i = 0; i < num; ++i) {
      DBG_ARR(fprintf(stderr, "asearrref destr %u %p\n", i,
        arr[i].get_rep().p));
      arr[i].~ase_variant(); /* DCHK */
    }
  }
  ase_variant *arr;
  int num;
};

struct luaimpl_getthisfromarg {
  void *operator ()(lua_State *luactx) {
    return lua_touserdata(luactx, 1);
  }
};

struct luaimpl_getthisfromup {
  void *operator ()(lua_State *luactx) {
    return lua_touserdata(luactx, lua_upvalueindex(1));
  }
};

template <typename G, typename F> int
luaimpl_handle_ase_exception(lua_State *luactx, G g, F f)
{
  ase_lua_aseproxy_privnode *p = static_cast<ase_lua_aseproxy_privnode *>(
    g(luactx));
  ase_lua_aseproxy_priv& self = p->value;
  try {
    return f(luactx, self);
  } catch (void *) {
    /* lua exception */
    throw;
  } catch (...) {
    ase_lua_util::handle_ase_exception(self.get_script_engine(), luactx);
  }
  return 0;
}

template <typename F> int
luaimpl_handle_ase_exception_sc(lua_State *luactx, F f)
{
  ase_lua_script_engine *sc = static_cast<ase_lua_script_engine *>(
    lua_touserdata(luactx, lua_upvalueindex(1)));
  try {
    return f(luactx, sc);
  } catch (void *) {
    /* lua exception */
    throw;
  } catch (...) {
    ase_lua_util::handle_ase_exception(sc, luactx);
  }
  return 0;
}

int
luaimpl_getdouble(lua_State *luactx, ase_lua_aseproxy_priv& self)
{
  ase_lua_script_engine::callfromlua_guard g(
    self, self.get_script_engine());
  double r = self.get_double();
  lua_pushnumber(luactx, r);
  return 1;
}

int
luaimpl_getstring(lua_State *luactx, ase_lua_aseproxy_priv& self)
{
  ase_lua_script_engine::callfromlua_guard g(
    self, self.get_script_engine());
  ase_string r = self.get_string();
  ase_lua_util::to_lua_string(luactx, r);
  return 1;
}

int
luaimpl_getprop(lua_State *luactx, ase_lua_aseproxy_priv& self)
{
  int ty = lua_type(luactx, 2);
  if (ty == LUA_TSTRING) {
    const char *name = lua_tostring(luactx, 2);
    ase_size_type namelen = lua_strlen(luactx, 2);
    const ase_int id = self.get_method_id(name, namelen);
    if (id != ase_method_id_not_found) {
      DBG(fprintf(stderr, "ASEPROXY pushclosure\n"));
      lua_pop(luactx, 1);
      lua_pushnumber(luactx, id);
      lua_pushcclosure(luactx, ase_lua_aseproxy_priv::aseproxy_invokebyid, 2);
      return 1;
    }
    ase_variant r;
    {
      ase_lua_script_engine::callfromlua_guard g(
	self, self.get_script_engine());
      r = self.get_property(name, namelen);
    }
    if (r.is_void() && ase_string::equals(name, namelen, "Keys")) {
      DBG(fprintf(stderr, "ASEPROXY pushnewenum\n"));
      lua_pushvalue(luactx, 1);
      lua_pushcclosure(luactx, ase_lua_aseproxy_priv::aseproxy_newenum, 1);
      return 1;
    }
    ase_lua_util::to_lua_value(self.get_script_engine(), luactx, r);
    return 1;
  } else if (ty == LUA_TNUMBER) {
    ase_int idx = static_cast<ase_int>(lua_tonumber(luactx, 2));
    ase_variant r;
    {
      ase_lua_script_engine::callfromlua_guard g(
	self, self.get_script_engine());
      r = self.get_element(idx);
    }
    ase_lua_util::to_lua_value(self.get_script_engine(), luactx, r);
    return 1;
  }
  return 0;
}

int
luaimpl_setprop(lua_State *luactx, ase_lua_aseproxy_priv& self)
{
  ase_variant val = ase_lua_util::to_ase_variant(self.get_script_engine(),
    luactx, 3);
  int ty = lua_type(luactx, 2);
  if (ty == LUA_TSTRING) {
    const char *name = lua_tostring(luactx, 2);
    ase_size_type namelen = lua_strlen(luactx, 2);
    {
      ase_lua_script_engine::callfromlua_guard g(
	self, self.get_script_engine());
      self.set_property(name, namelen, val);
    }
    return 0;
  } else if (ty == LUA_TNUMBER) {
    ase_int idx = static_cast<ase_int>(lua_tonumber(luactx, 2));
    {
      ase_lua_script_engine::callfromlua_guard g(
	self, self.get_script_engine());
      self.set_element(idx, val);
    }
    return 0;
  }
  return 0;
}

int
luaimpl_invoke(lua_State *luactx, ase_lua_aseproxy_priv& self)
{
  ase_size_type nargs = lua_gettop(luactx) - 1;
  DBG(fprintf(stderr, "ASEPROXY invokeself nargs=%d\n", (int)nargs));
  ase_lua_script_engine *sc = self.get_script_engine();
  ase_variant *aargs = static_cast<ase_variant *>(
    ASE_JS_STACK_ALLOCA((nargs + 1) * sizeof(ase_variant)));
      /* +1 to avoid alloca(0) */
  ase_lua_asearrref arrref;
  arrref.set(aargs, static_cast<int>(nargs), 2, sc, luactx);
  ase_variant r;
  {
    ase_lua_script_engine::callfromlua_guard g(
      self, self.get_script_engine());
    r = self.invoke_self(aargs, nargs);
  }
  ase_lua_util::to_lua_value(sc, luactx, r);
  DBG(fprintf(stderr, "ASEPROXY invokeself done\n"));
  return 1;
}

int
luaimpl_eq(lua_State *luactx, ase_lua_aseproxy_priv& self)
{
  ase_variant y = ase_lua_util::to_ase_variant(self.get_script_engine(),
    luactx, 2);
  bool r = (self == y);
  lua_pushboolean(luactx, r);
  return 1;
}

int
luaimpl_invokebyid(lua_State *luactx, ase_lua_aseproxy_priv& self)
{
  ase_lua_script_engine *sc = self.get_script_engine();
  ase_int id = static_cast<ase_int>(lua_tonumber(luactx,
    lua_upvalueindex(2)));
  ase_size_type nargs = lua_gettop(luactx);
  ase_variant *aargs = static_cast<ase_variant *>(
    ASE_JS_STACK_ALLOCA((nargs + 1) * sizeof(ase_variant)));
      /* +1 to avoid alloca(0) */
  ase_lua_asearrref arrref;
  arrref.set(aargs, static_cast<int>(nargs), 1, sc, luactx);
  if ((self.get_vtoption() & ase_vtoption_multithreaded) == 0) {
    ase_variant r = self.invoke_by_id(id, aargs, nargs);
    ase_lua_util::to_lua_value(sc, luactx, r);
  } else {
    ase_variant r;
    {
      ase_lua_script_engine::callfromlua_guard g(
	self, self.get_script_engine());
      r = self.invoke_by_id(id, aargs, nargs);
    }
    ase_lua_util::to_lua_value(sc, luactx, r);
  } 
  return 1;
}

int
luaimpl_invokebyid_unbound(lua_State *luactx, ase_lua_aseproxy_priv& kself)
{
  ase_lua_script_engine *sc = kself.get_script_engine();
  ase_int id = static_cast<ase_int>(lua_tonumber(luactx,
    lua_upvalueindex(2)));
  ase_size_type nargs = lua_gettop(luactx);
  void *ud = 0;
  if (nargs < 1 || (ud = lua_touserdata(luactx, 1)) == 0 ||
    lua_getmetatable(luactx, 1) == 0) {
    ase_throw_invalid_arg("Lua: unbound method: arg #1 must be an object");
  }
  const void *const mtptr = lua_topointer(luactx, -1);
  if (mtptr != sc->get_asepr_metatable()) {
    ase_throw_type_mismatch(
      "Lua: unbound method: arg #1: incompatible object");
  }
  ase_lua_aseproxy_privnode *p = static_cast<ase_lua_aseproxy_privnode *>(
    ud);
  ase_lua_aseproxy_priv& self = p->value;
  if (kself.get_instance_vtable_address() != self.get_vtable_address()) {
    ase_throw_type_mismatch(
      "Lua: unbound method: arg #1: incompatible object");
  }

  --nargs;
  ase_variant *aargs = static_cast<ase_variant *>(
    ASE_JS_STACK_ALLOCA((nargs + 1) * sizeof(ase_variant)));
      /* +1 to avoid alloca(0) */
  ase_lua_asearrref arrref;
  arrref.set(aargs, static_cast<int>(nargs), 2, sc, luactx);
  if ((self.get_vtoption() & ase_vtoption_multithreaded) == 0) {
    ase_variant r = self.invoke_by_id(id, aargs, nargs);
    ase_lua_util::to_lua_value(sc, luactx, r);
  } else {
    ase_variant r;
    {
      ase_lua_script_engine::callfromlua_guard g(
	self, self.get_script_engine());
      r = self.invoke_by_id(id, aargs, nargs);
    }
    ase_lua_util::to_lua_value(sc, luactx, r);
  } 
  return 1;
}

int
luaimpl_invokebyname(lua_State *luactx, ase_lua_aseproxy_priv& self)
{
  ase_lua_script_engine *sc = self.get_script_engine();
  int upidx = lua_upvalueindex(2);
  const char *name = lua_tostring(luactx, upidx);
  ase_size_type namelen = lua_strlen(luactx, upidx);
  ase_size_type nargs = lua_gettop(luactx);
  ase_variant *aargs = static_cast<ase_variant *>(
    ASE_JS_STACK_ALLOCA((nargs + 1) * sizeof(ase_variant)));
      /* +1 to avoid alloca(0) */
  ase_lua_asearrref arrref;
  arrref.set(aargs, static_cast<int>(nargs), 1, sc, luactx);
  if ((self.get_vtoption() & ase_vtoption_multithreaded) == 0) {
    ase_variant r = self.invoke_by_name(name, namelen, aargs, nargs);
    ase_lua_util::to_lua_value(sc, luactx, r);
  } else {
    ase_variant r;
    {
      ase_lua_script_engine::callfromlua_guard g(
	self, self.get_script_engine());
      r = self.invoke_by_name(name, namelen, aargs, nargs);
    }
    ase_lua_util::to_lua_value(sc, luactx, r);
  } 
  return 1;
}

int
luaimpl_newenum(lua_State *luactx, ase_lua_aseproxy_priv& self)
{
  ase_lua_script_engine *sc = self.get_script_engine();
  ase_variant r;
  {
    ase_lua_script_engine::callfromlua_guard g(
      self, self.get_script_engine());
    r = self.get_enumerator();
  }
  ase_lua_util::to_lua_value(sc, luactx, r);
  lua_pushcclosure(luactx, ase_lua_aseproxy_priv::aseproxy_enumnext, 1);
  lua_pushnil(luactx);
  lua_pushnil(luactx);
  return 3; /* iter, nil, nil */
}

int
luaimpl_enumnext(lua_State *luactx, ase_lua_aseproxy_priv& self)
{
  bool hasnext = false;
  ase_string rk;
  {
    ase_lua_script_engine::callfromlua_guard g(
      self, self.get_script_engine());
    rk = self.enum_next(hasnext);
  }
  if (hasnext) {
    ase_lua_util::to_lua_string(luactx, rk);
  } else {
    lua_pushnil(luactx);
  }
  return 1;
}

int
luaimpl_import_klass(lua_State *luactx, ase_lua_script_engine *sc)
{
  int nargs = lua_gettop(luactx);
  if (nargs < 3) {
    ase_throw_missing_arg("Lua: Usage: Import(table or nil, klass, prefix)");
    return 0;
  }
  bool import_global = false;
  if (lua_isnil(luactx, 1)) {
    import_global = true;
  } else if (!lua_istable(luactx, 1)) {
    ase_throw_type_mismatch("Lua: Usage: Import(table or nil, klass, prefix)");
  }
  ase_variant klass = ase_lua_util::to_ase_variant(sc, luactx, 2);
  ase_string prefix =
    ase_lua_util::to_ase_variant(sc, luactx, 3).get_string();
  ase_size_type nkmtds = klass.get_num_methods();
  ase_size_type nimtds = klass.get_num_instance_methods();
  const ase_variant_method_entry *const kmtds = klass.get_methods();
  const ase_variant_method_entry *const imtds = klass.get_instance_methods();
  /* import klass methods */
  for (ase_size_type i = 0; i < nkmtds; ++i) {
    ase_string n = prefix + ase_string(kmtds[i].u8name);
    ase_size_type id = static_cast<ase_size_type>(i);
    DBG_IMPORT(fprintf(stderr, "klass=%s\n", klass.get_string().data()));
    DBG_IMPORT(fprintf(stderr, "kmethod=%s\n", n.data()));
    lua_pushvalue(luactx, 2); /* klass */
    lua_pushinteger(luactx, id); /* id */
    lua_pushcclosure(luactx, ase_lua_aseproxy_priv::aseproxy_invokebyid, 2);
    if (import_global) {
      lua_setglobal(luactx, n.data());
    } else {
      lua_setfield(luactx, 1, n.data());
    }
  }
  /* import instance methods */
  for (ase_size_type i = 0; i < nimtds; ++i) {
    ase_string n = prefix + ase_string(imtds[i].u8name);
    ase_size_type id = static_cast<ase_size_type>(i);
    DBG_IMPORT(fprintf(stderr, "klass=%s\n", klass.get_string().data()));
    DBG_IMPORT(fprintf(stderr, "imethod=%s\n", n.data()));
    lua_pushvalue(luactx, 2); /* klass */
    lua_pushinteger(luactx, id); /* id */
    lua_pushcclosure(luactx,
      ase_lua_aseproxy_priv::aseproxy_invokebyid_unbound, 2);
    if (import_global) {
      lua_setglobal(luactx, n.data());
    } else {
      lua_setfield(luactx, 1, n.data());
    }
  }
  /* import the constructor */
  {
    ase_string cn = prefix + ase_string("NewInstance");
    lua_pushvalue(luactx, 2); /* klass */
    if (import_global) {
      lua_setglobal(luactx, cn.data());
    } else {
      lua_setfield(luactx, 1, cn.data());
    }
  }
  /* return value */
  lua_pushvalue(luactx, 1); /* table or nil */
  return 1;
}

int
luaimpl_create_unbound(lua_State *luactx, ase_lua_script_engine *sc)
{
  int nargs = lua_gettop(luactx);
  if (nargs < 2) {
    ase_throw_missing_arg("Lua: Usage: CreateUnbound(klass, name)");
  }
  ase_variant klass = ase_lua_util::to_ase_variant(sc, luactx, 1);
  ase_string name = ase_lua_util::to_ase_variant(sc, luactx, 2).get_string();
  ase_int id = klass.get_instance_method_id(name);
  DBG_UNBOUND(fprintf(stderr, "klass=%s\n", klass.get_string().data()));
  DBG_UNBOUND(fprintf(stderr, "method=%s\n", name.data()));
  if (id == ase_method_id_not_found) {
    ase_throw_invalid_arg("Lua: CreateUnbound: method not found");
  }
  DBG_UNBOUND(fprintf(stderr, "methodid=%d\n", id));
  lua_pushvalue(luactx, 1); /* klass */
  lua_pushinteger(luactx, id); /* id */
  lua_pushcclosure(luactx,
    ase_lua_aseproxy_priv::aseproxy_invokebyid_unbound, 2);
  return 1;
}

}; // anonymous namespace

int
ase_lua_aseproxy_priv::aseproxy_getdouble(lua_State *luactx)
{
  return luaimpl_handle_ase_exception(luactx, luaimpl_getthisfromarg(),
    luaimpl_getdouble);
}

int
ase_lua_aseproxy_priv::aseproxy_getstring(lua_State *luactx)
{
  return luaimpl_handle_ase_exception(luactx, luaimpl_getthisfromarg(),
    luaimpl_getstring);
}

int
ase_lua_aseproxy_priv::aseproxy_getprop(lua_State *luactx)
{
  return luaimpl_handle_ase_exception(luactx, luaimpl_getthisfromarg(),
    luaimpl_getprop);
}

int
ase_lua_aseproxy_priv::aseproxy_setprop(lua_State *luactx)
{
  return luaimpl_handle_ase_exception(luactx, luaimpl_getthisfromarg(),
    luaimpl_setprop);
}

int
ase_lua_aseproxy_priv::aseproxy_invoke(lua_State *luactx)
{
  return luaimpl_handle_ase_exception(luactx, luaimpl_getthisfromarg(),
    luaimpl_invoke);
}

int
ase_lua_aseproxy_priv::aseproxy_eq(lua_State *luactx)
{
  return luaimpl_handle_ase_exception(luactx, luaimpl_getthisfromarg(),
    luaimpl_eq);
}

int
ase_lua_aseproxy_priv::aseproxy_invokebyid(lua_State *luactx)
{
  /* up: [ obj, id ] stack: [ args... ] */
  return luaimpl_handle_ase_exception(luactx, luaimpl_getthisfromup(),
    luaimpl_invokebyid);
}

int
ase_lua_aseproxy_priv::aseproxy_import_klass(lua_State *luactx)
{
  /* up: [ sctx ] stack: [ table, klassobj, prefix ] */
  return luaimpl_handle_ase_exception_sc(luactx, luaimpl_import_klass);
}

int
ase_lua_aseproxy_priv::aseproxy_create_unbound(lua_State *luactx)
{
  /* up: [ sctx ] stack: [ klassobj, name ] */
  return luaimpl_handle_ase_exception_sc(luactx, luaimpl_create_unbound);
}

int
ase_lua_aseproxy_priv::aseproxy_invokebyid_unbound(lua_State *luactx)
{
  /* up: [ klassobj, id ] stack: [ obj, args... ] */
  return luaimpl_handle_ase_exception(luactx, luaimpl_getthisfromup(),
    luaimpl_invokebyid_unbound);
}

int
ase_lua_aseproxy_priv::aseproxy_invokebyname(lua_State *luactx)
{
  /* up: [ obj, name ] stack: [ args... ] */
  return luaimpl_handle_ase_exception(luactx, luaimpl_getthisfromup(),
    luaimpl_invokebyname);
}

int
ase_lua_aseproxy_priv::aseproxy_newenum(lua_State *luactx)
{
  /* up: [ obj ] stack: [ ] */
  return luaimpl_handle_ase_exception(luactx, luaimpl_getthisfromup(),
    luaimpl_newenum);
}

int
ase_lua_aseproxy_priv::aseproxy_enumnext(lua_State *luactx)
{
  /* up: [ obj ] stack: [ ] */
  return luaimpl_handle_ase_exception(luactx, luaimpl_getthisfromup(),
    luaimpl_enumnext);
}

}; // namespace aselua

