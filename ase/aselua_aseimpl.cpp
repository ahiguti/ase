
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/aseflags.hpp>
#include <ase/aseexcept.hpp>

#include "aselua_util.hpp"
#include "aselua_aseimpl.hpp"

#include <cassert>

#ifdef _MSC_VER
#include <malloc.h>
#define ASE_LUA_STACK_ALLOCA(x) _alloca(x)
#else
#include <alloca.h>
#define ASE_LUA_STACK_ALLOCA(x) alloca(x)
#endif

#define DBG_CONSTR(x)
#define DBG_DESTROY(x)
#define DBG_RELEASE(x)
#define DBG_IMPL(x)
#define DBG(x)
#define DBG_DIS(x)

namespace aselua {

ase_lua_ase_variant_impl::ase_lua_ase_variant_impl(const init_arg& arg)
  : refcount(1)
{
  lua_State *luactx = arg.luactx;
  int valindex = arg.valindex;
  lua_pushlightuserdata(luactx, this);
  if (valindex) {
    if (valindex < 0) {
      --valindex;
    }
    lua_pushvalue(luactx, valindex);
  } else {
    lua_pushnil(luactx);
  }
  lua_rawset(luactx, LUA_REGISTRYINDEX);
  DBG_CONSTR(fprintf(stderr, "lua: CONSTR %p\n", this));
  ASE_DEBUG(ase_lua_variant_impl_count_constr++);
}

ase_lua_ase_variant_impl::~ase_lua_ase_variant_impl() /* DCHK */
{
  ASE_DEBUG(ase_lua_variant_impl_count_destr++);
  DBG_DESTROY(fprintf(stderr, "lua: DESTROY %p\n", this));
}

void
ase_lua_ase_variant_impl::add_ref(const ase_variant& self)
{
  ase_lua_ase_variant_implnode *const selfp = get(self);
  DBG_RELEASE(fprintf(stderr, "LUA: ADDREF %p %d\n", selfp,
    selfp->value.refcount));
  selfp->value.refcount++;
}

void
ase_lua_ase_variant_impl::release(const ase_variant& self)
{
  ase_lua_ase_variant_implnode *const selfp = get(self);
  DBG_RELEASE(fprintf(stderr, "LUA: RELEASE %p %d\n", selfp,
    selfp->value.refcount));
  if ((selfp->value.refcount--) == 1) {
    ase_lua_script_engine *const sc = getctx(self);
    ase_lua_script_engine::callfromase_guard g(sc);
    lua_State *luactx = g.get_current_luactx();
    DBG_DIS(fprintf(stderr, "LUAPR release %p\n", selfp));
    DBG_DIS(fprintf(stderr, "aseim release gettop=%d\n", lua_gettop(luactx)));
    selfp->value.dispose(sc, luactx);
    ase_variant::debug_on_destroy_object(self, "LuaObject");
    delete selfp;
  }
}

void
ase_lua_ase_variant_impl::dispose(ase_lua_script_engine *sc,
  lua_State *luactx)
{
  DBG_DIS(fprintf(stderr, "LUAPR dispose %p\n", this));
  DBG_DIS(fprintf(stderr, "aseim dispose gettop=%d\n", lua_gettop(luactx)));
  lua_pushlightuserdata(luactx, this);
  lua_pushnil(luactx);
  lua_rawset(luactx, LUA_REGISTRYINDEX);
}

void
luacb_protected_internal(const ase_variant& self, lua_CFunction cb, void *ud,
  const char *debug_id)
{
  DBG_IMPL(fprintf(stderr, "LUACB %s begin\n", debug_id));
  ase_lua_script_engine *const sc = ase_lua_ase_variant_impl::getctx(self);
  ase_lua_script_engine::callfromase_guard g(sc);
  lua_State *const luactx = g.get_current_luactx();
  int e = lua_cpcall(luactx, cb, ud);
  if (e) {
    if (e == LUA_ERRMEM) {
      lua_pop(luactx, 1);
      throw ase_new_out_of_memory_exception();
    }
    ase_variant ex = ase_lua_util::to_ase_variant(sc, luactx, -1);
    lua_pop(luactx, 1);
    throw ase_convert_to_ase_exception(ase_string("Lua/"), ase_string(),
      ase_string(), ex);
  }
  DBG_IMPL(fprintf(stderr, "LUACB %s end\n", debug_id));
}

template <typename T> struct luacb_tmpl {
  static int call(lua_State *luactx) {
    T& arg = *static_cast<T *>(lua_touserdata(luactx, 1));
    lua_pop(luactx, 1);
    ase_lua_ase_variant_implnode *const selfp =
      ase_lua_ase_variant_impl::get(arg.self);
    ase_lua_ase_variant_impl *const regkey =
      ase_lua_ase_variant_impl::get_regkey(selfp);
    ase_lua_script_engine *const sc =
      ase_lua_ase_variant_impl::getctx(arg.self);
    try {
      lua_pushlightuserdata(luactx, regkey);
      lua_rawget(luactx, LUA_REGISTRYINDEX);
      arg(sc, luactx);
    } catch (void *) {
      /* lua exception */
      throw;
    } catch (...) {
      ase_lua_util::handle_ase_exception(sc, luactx);
    }
    return 0;
  }
};

template <typename T> void
luacb_protected_tmpl(T& arg, const char *debug_id)
{
  luacb_protected_internal(arg.self, luacb_tmpl<T>::call, &arg, debug_id);
}

struct luacb_getattributes_arg {
  luacb_getattributes_arg(const ase_variant& s)
    : self(s), retval(0) { }
  const ase_variant& self;
  ase_int retval;
  void operator ()(ase_lua_script_engine *sc, lua_State *luactx) {
    retval = ase_attrib_mutable;
    int ty = lua_type(luactx, -1);
    if (ty == LUA_TFUNCTION) {
      retval |= ase_attrib_function;
    }
  }
};

ase_int
ase_lua_ase_variant_impl::get_attributes(const ase_variant& self)
{
  luacb_getattributes_arg arg(self);
  luacb_protected_tmpl(arg, "getattr");
  return arg.retval;
}

struct luacb_getnumber_arg {
  luacb_getnumber_arg(const ase_variant& s)
    : self(s), retval(0) { }
  const ase_variant& self;
  double retval;
  void operator ()(ase_lua_script_engine *sc, lua_State *luactx) {
    if (luaL_callmeta(luactx, -1, "__tonumber")) {
      retval = lua_tonumber(luactx, -1);
      return;
    }
    retval = 0;
  }
};

bool
ase_lua_ase_variant_impl::get_boolean(const ase_variant& self)
{
  return 0;
}

ase_int
ase_lua_ase_variant_impl::get_int(const ase_variant& self)
{
  luacb_getnumber_arg arg(self);
  luacb_protected_tmpl(arg, "getint");
  return static_cast<ase_int>(arg.retval);
}

ase_long
ase_lua_ase_variant_impl::get_long(const ase_variant& self)
{
  luacb_getnumber_arg arg(self);
  luacb_protected_tmpl(arg, "getlong");
  return static_cast<ase_long>(arg.retval);
}

double
ase_lua_ase_variant_impl::get_double(const ase_variant& self)
{
  luacb_getnumber_arg arg(self);
  luacb_protected_tmpl(arg, "getdouble");
  return arg.retval;
}

struct luacb_getstring_arg {
  luacb_getstring_arg(const ase_variant& s)
    : self(s) { }
  const ase_variant& self;
  ase_string retval;
  void operator ()(ase_lua_script_engine *sc, lua_State *luactx) {
    if (luaL_callmeta(luactx, -1, "__tostring")) {
      retval = ase_lua_util::to_ase_string(luactx, -1);
      return;
    }
    retval = ase_string("[LuaObject]");
  }
};

ase_string
ase_lua_ase_variant_impl::get_string(const ase_variant& self)
{
  luacb_getstring_arg arg(self);
  luacb_protected_tmpl(arg, "getstring");
  return arg.retval;
}

bool
ase_lua_ase_variant_impl::is_same_object(const ase_variant& self,
  const ase_variant& value)
{
  ase_lua_script_engine *const sc =
    ase_lua_ase_variant_impl::getctx(self);
  if (!ase_lua_util::is_lua_proxy(sc, value)) {
    return false;
  }
  ase_lua_ase_variant_implnode *const selfp =
    ase_lua_ase_variant_impl::get(self);
  ase_lua_ase_variant_impl *const selfregkey =
    ase_lua_ase_variant_impl::get_regkey(selfp);
  ase_lua_ase_variant_implnode *const ovp =
    ase_lua_ase_variant_impl::get(value);
  ase_lua_ase_variant_impl *const ovregkey =
    ase_lua_ase_variant_impl::get_regkey(ovp);
  return selfregkey == ovregkey;
}

struct luacb_getproperty_arg {
  luacb_getproperty_arg(const ase_variant& s, const char *n,
    ase_size_type nlen)
    : self(s), name(n), namelen(nlen) { }
  const ase_variant& self;
  const char *const name;
  const ase_size_type namelen;
  ase_variant retval;
  void operator ()(ase_lua_script_engine *sc, lua_State *luactx) {
    lua_pushlstring(luactx, name, namelen);
    lua_gettable(luactx, -2);
    retval = ase_lua_util::to_ase_variant(sc, luactx, -1);
    DBG(ase_lua_util::dump_gettop(luactx, "luapr::getprop"));
  }
};

ase_variant
ase_lua_ase_variant_impl::get_property(const ase_variant& self,
  const char *name, ase_size_type namelen)
{
  luacb_getproperty_arg arg(self, name, namelen);
  luacb_protected_tmpl(arg, "getproperty");
  return arg.retval;
}

struct luacb_setproperty_arg {
  luacb_setproperty_arg(const ase_variant& s, const char *n,
    ase_size_type nlen, const ase_variant& v)
      : self(s), name(n), namelen(nlen), value(v) { }
  const ase_variant& self;
  const char *const name;
  const ase_size_type namelen;
  const ase_variant& value;
  void operator ()(ase_lua_script_engine *sc, lua_State *luactx) {
    lua_pushlstring(luactx, name, namelen);
    ase_lua_util::to_lua_value(sc, luactx, value);
    lua_settable(luactx, -3);
  }
};

void
ase_lua_ase_variant_impl::set_property(const ase_variant& self,
  const char *name, ase_size_type namelen, const ase_variant& value)
{
  luacb_setproperty_arg arg(self, name, namelen, value);
  luacb_protected_tmpl(arg, "setproperty");
}

struct luacb_delproperty_arg {
  luacb_delproperty_arg(const ase_variant& s, const char *n,
    ase_size_type nlen)
      : self(s), name(n), namelen(nlen) { }
  const ase_variant& self;
  const char *const name;
  const ase_size_type namelen;
  void operator ()(ase_lua_script_engine *sc, lua_State *luactx) {
    lua_pushlstring(luactx, name, namelen);
    lua_pushnil(luactx);
    lua_settable(luactx, -3);
  }
};

void
ase_lua_ase_variant_impl::del_property(const ase_variant& self,
  const char *name, ase_size_type namelen)
{
  luacb_delproperty_arg arg(self, name, namelen);
  luacb_protected_tmpl(arg, "delproperty");
}

struct luacb_getelement_arg {
  luacb_getelement_arg(const ase_variant& s, ase_int i)
    : self(s), index(i) { }
  const ase_variant& self;
  const ase_int index;
  ase_variant retval;
  void operator ()(ase_lua_script_engine *sc, lua_State *luactx) {
    DBG(fprintf(stderr, "LUA: ASEIMPL get_element %d\n", index));
    /* FIXME: ARGUABLE */
    /* ASE is 0-based. lua is 1-based */
    lua_pushnumber(luactx, static_cast<lua_Number>(index + 1));
    lua_gettable(luactx, -2);
    retval = ase_lua_util::to_ase_variant(sc, luactx, -1);
    DBG(ase_lua_util::dump_gettop(luactx, "luapr::getelem"));
  }
};

ase_variant
ase_lua_ase_variant_impl::get_element(const ase_variant& self, ase_int index)
{
  luacb_getelement_arg arg(self, index);
  luacb_protected_tmpl(arg, "getelement");
  return arg.retval;
}

struct luacb_setelement_arg {
  luacb_setelement_arg(const ase_variant& s, ase_int i, const ase_variant& v)
    : self(s), index(i), value(v) { }
  const ase_variant& self;
  const ase_int index;
  const ase_variant& value;
  void operator ()(ase_lua_script_engine *sc, lua_State *luactx) {
    DBG(fprintf(stderr, "LUA: ASEIMPL set_element %d\n", index));
    /* FIXME: ARGUABLE */
    /* ASE is 0-based. lua is 1-based */
    lua_pushnumber(luactx, static_cast<lua_Number>(index + 1));
    ase_lua_util::to_lua_value(sc, luactx, value);
    lua_settable(luactx, -3);
  }
};

void
ase_lua_ase_variant_impl::set_element(const ase_variant& self, ase_int index,
  const ase_variant& value)
{
  luacb_setelement_arg arg(self, index, value);
  luacb_protected_tmpl(arg, "setelement");
}

struct luacb_getlength_arg {
  luacb_getlength_arg(const ase_variant& s) : self(s), len(-1) { }
  const ase_variant& self;
  ase_int len;
  void operator ()(ase_lua_script_engine *sc, lua_State *luactx) {
    DBG(fprintf(stderr, "LUA: ASEIMPL get_length\n"));
    size_t l = lua_objlen(luactx, -1);
    len = static_cast<ase_int>(l);
  }
};

ase_int
ase_lua_ase_variant_impl::get_length(const ase_variant& self)
{
  luacb_getlength_arg arg(self);
  luacb_protected_tmpl(arg, "getlength");
  return arg.len;
}

void
ase_lua_ase_variant_impl::set_length(const ase_variant& self, ase_int len)
{
}

ase_variant
ase_lua_ase_variant_impl::invoke_by_name(const ase_variant& self,
  const char *name, ase_size_type namelen, const ase_variant *args,
  ase_size_type nargs)
{
  ase_variant pv = get_property(self, name, namelen);
  return pv.invoke_self(args, nargs);
}

ase_variant
ase_lua_ase_variant_impl::invoke_by_id(const ase_variant& self, ase_int id,
  const ase_variant *args, ase_size_type nargs)
{
  return ase_variant();
}

struct luacb_invokeself_arg {
  luacb_invokeself_arg(const ase_variant& s, const ase_variant *a,
    ase_size_type n)
    : self(s), args(a), nargs(n) { }
  const ase_variant& self;
  const ase_variant *const args;
  const ase_size_type nargs;
  ase_variant retval;
  void operator ()(ase_lua_script_engine *sc, lua_State *luactx) {
    for (ase_size_type i = 0; i < nargs; ++i) {
      ase_lua_util::to_lua_value(sc, luactx, args[i]);
    }
    lua_call(luactx, static_cast<int>(nargs), 1);
    retval = ase_lua_util::to_ase_variant(sc, luactx, -1);
  }
};

ase_variant
ase_lua_ase_variant_impl::invoke_self(const ase_variant& self,
  const ase_variant *args, ase_size_type nargs)
{
  luacb_invokeself_arg arg(self, args, nargs);
  luacb_protected_tmpl(arg, "invokeself");
  return arg.retval;
}

struct luacb_getenumerator_arg {
  luacb_getenumerator_arg(const ase_variant& s) : self(s) { }
  const ase_variant& self;
  ase_variant retval;
  void operator ()(ase_lua_script_engine *sc, lua_State *luactx) {
    lua_newtable(luactx);
    lua_pushvalue(luactx, -2);
    lua_rawseti(luactx, -2, 0); /* tbl[0] = t */
    lua_pushnil(luactx);
    lua_rawseti(luactx, -2, 1); /* tbl[1] = nil */
    assert(lua_istable(luactx, -1));
    retval = ase_lua_util::to_ase_variant(sc, luactx, -1);
  }
};

ase_variant
ase_lua_ase_variant_impl::get_enumerator(const ase_variant& self)
{
  luacb_getenumerator_arg arg(self);
  luacb_protected_tmpl(arg, "getenum");
  return arg.retval;
}

struct luacb_enumnext_arg {
  luacb_enumnext_arg(const ase_variant& s) : self(s), hasnext(false) { }
  const ase_variant& self;
  ase_string retval;
  bool hasnext;
  void operator ()(ase_lua_script_engine *sc, lua_State *luactx) {
    /* [tbl] */
    if (!lua_istable(luactx, -1)) {
      return;
    }
    lua_rawgeti(luactx, -1, 0); /* t = tbl[0] */
    /* [tbl, t] */
    if (!lua_istable(luactx, -1)) {
      return;
    }
    lua_rawgeti(luactx, -2, 1); /* k = tbl[1] */
    /* [tbl, t, k] */
    while (lua_next(luactx, -2)) {
      /* [tbl, t, nk, nv] */
      lua_pop(luactx, 1);
      int ty = lua_type(luactx, -1);
      /* [tbl, t, nk] */
      if (ty == LUA_TSTRING) {
	retval = ase_lua_util::to_ase_string(luactx, -1);
	lua_rawseti(luactx, -3, 1); /* tbl[1] = k */
	hasnext = true;
	return;
      }
      /* skip non-string key */
    }
  }
};

ase_string
ase_lua_ase_variant_impl::enum_next(const ase_variant& self, bool& hasnext_r)
{
  hasnext_r = false;
  luacb_enumnext_arg arg(self);
  luacb_protected_tmpl(arg, "enumnext");
  hasnext_r = arg.hasnext;
  return arg.retval;
}

}; // namespace aselua

