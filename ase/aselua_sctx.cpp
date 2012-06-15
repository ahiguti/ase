
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asefuncobj.hpp>
#include <ase/aseio.hpp>
#include <ase/aseexcept.hpp>

#include "aselua_util.hpp"
#include "aselua_aseimpl.hpp"
#include "aselua_asepr.hpp"

#define DBG_IMPORT(x)
#define DBG_STACK(x)
#define DBG_CTX(x)
#define DBG_EVAL(x)
#define DBG(x)

namespace aselua {

int
func_eval(lua_State *luactx)
{
  DBG_EVAL(fprintf(stderr, "LUA EVAL begin\n"));
  const char *src = lua_tostring(luactx, 1);
  size_t srclen = lua_strlen(luactx, 1);
  const char *fname = src;
  if (lua_gettop(luactx) > 1) {
    fname = lua_tostring(luactx, 2);
    lua_pushfstring(luactx, "@%s", fname);
    fname = lua_tostring(luactx, -1);
  }
  if (luaL_loadbuffer(luactx, src, srclen, fname) != 0) {
    lua_error(luactx);
  }
  lua_call(luactx, 0, 1);
  DBG_EVAL(fprintf(stderr, "LUA EVAL end\n"));
  return 1;
}

ase_lua_script_engine::ase_lua_script_engine(ase_unified_engine_ref& ue)
  : uemutex(ue.get_mutex()),
    variant_vtable(ase_variant_impl<ase_lua_ase_variant_impl>::vtable),
    default_luactx(0), asepr_metatable(0), lua_endnode(0), ase_endnode(0)
{
  try {
    init();
  } catch (const std::bad_alloc&) {
    cleanup();
    throw;
  } catch (...) {
    cleanup();
    /* we must not rethrow the thrown object because it may refer to
      'this'. */
    ase_throw_error("Lua: failed to initialize lua engine");
  }
}

ase_lua_script_engine::~ase_lua_script_engine() /* DCHK */
{
  cleanup();
}

void
ase_lua_script_engine::init()
{
  variant_vtable.impl_private_data = this;
  lua_State *luactx = luaL_newstate();
  DBG_CTX(fprintf(stderr, "LUA: newstate: %p\n", luactx));
  if (!luactx) {
    ase_throw_error("Lua: failed to create lua engine");
  }
  default_luactx = luactx;
  lua_endnode = new ase_lua_ase_variant_implnode(
    ase_lua_ase_variant_impl::init_arg(this, luactx, 0));
  DBG_CTX(fprintf(stderr, "luaendnode=%p\n", lua_endnode));
  ase_endnode = new ase_lua_aseproxy_privnode(
    ase_lua_aseproxy_priv::init_arg(this, ase_variant()));
  DBG_CTX(fprintf(stderr, "aseendnode=%p\n", ase_endnode));
  /* base library */
  luaL_openlibs(luactx);
  /* create metatable for aseproxy */
  void *const metatable_key = this;
  lua_pushlightuserdata(luactx, metatable_key);
  lua_newtable(luactx);
  asepr_metatable = lua_topointer(luactx, -1);
  /* register meta functions */
  lua_pushstring(luactx, "__metatable"); /* for getmetatable() */
  lua_pushstring(luactx, "HIDDEN_METATABLE");
  lua_rawset(luactx, -3);
  lua_pushstring(luactx, "__gc");
  lua_pushcfunction(luactx, ase_lua_aseproxy_priv::aseproxy_finalize);
  lua_rawset(luactx, -3);
  lua_pushstring(luactx, "__index");
  lua_pushcfunction(luactx, ase_lua_aseproxy_priv::aseproxy_getprop);
  lua_rawset(luactx, -3);
  lua_pushstring(luactx, "__newindex");
  lua_pushcfunction(luactx, ase_lua_aseproxy_priv::aseproxy_setprop);
  lua_rawset(luactx, -3);
  lua_pushstring(luactx, "__call");
  lua_pushcfunction(luactx, ase_lua_aseproxy_priv::aseproxy_invoke);
  lua_rawset(luactx, -3);
  lua_pushstring(luactx, "__tonumber");
  lua_pushcfunction(luactx, ase_lua_aseproxy_priv::aseproxy_getdouble);
  lua_rawset(luactx, -3);
  lua_pushstring(luactx, "__tostring");
  lua_pushcfunction(luactx, ase_lua_aseproxy_priv::aseproxy_getstring);
  lua_rawset(luactx, -3);
  lua_pushstring(luactx, "__eq");
  lua_pushcfunction(luactx, ase_lua_aseproxy_priv::aseproxy_eq);
  lua_rawset(luactx, -3);
  /* register mtbl with the registry */
  lua_rawset(luactx, LUA_REGISTRYINDEX);
  /* 'EvalString' function */
  lua_pushstring(luactx, "EvalString");
  lua_pushcfunction(luactx, func_eval);
  lua_settable(luactx, LUA_GLOBALSINDEX);
  /* 'ASELuaUtil' namespace */
  lua_newtable(luactx);
  lua_pushvalue(luactx, -1);
  lua_setglobal(luactx, "ASELuaUtil");
  /*  'ASELuaUtil.CreateUnbound' function */
  lua_pushlightuserdata(luactx, this);
  lua_pushcclosure(luactx, ase_lua_aseproxy_priv::aseproxy_create_unbound, 1);
  lua_setfield(luactx, -2, "CreateUnbound");
  /*  'ASELuaUtil.Import' function */
  lua_pushlightuserdata(luactx, this);
  lua_pushcclosure(luactx, ase_lua_aseproxy_priv::aseproxy_import_klass, 1);
  lua_setfield(luactx, -2, "Import");
  lua_pop(luactx, 1); /* ASELuaUtil table */
  /* get global object */
  lua_pushvalue(luactx, LUA_GLOBALSINDEX);
  luaglobal = ase_lua_util::to_ase_variant(this, luactx, -1);
  DBG_CTX(fprintf(stderr, "luagl rep=%p\n", luaglobal.get_rep().p));
  lua_pop(luactx, 1);
  DBG_STACK(ase_lua_util::dump_gettop(luactx, "newsctx"));
}

void
ase_lua_script_engine::cleanup()
{
  DBG_CTX(fprintf(stderr, "stacksize=%d\n", lua_gettop(default_luactx)));
  dispose_proxies();
  luaglobal = ase_variant();
  for (ase_size_type i = 0; i < cached_contexts.size(); ++i) {
    lua_State *luactx = cached_contexts[i];
    /* remove luactx from the registry */
    lua_pushthread(luactx);
    lua_pushnil(luactx);
    lua_settable(luactx, LUA_REGISTRYINDEX); /* unuse luactx */
    DBG_CTX(fprintf(stderr, "LUA: closethread: %p\n", luactx));
  }
  delete ase_endnode;
  delete lua_endnode;
  if (default_luactx) {
    DBG_STACK(ase_lua_util::dump_gettop(default_luactx, "delsctx"));
    DBG_CTX(fprintf(stderr, "LUA: closestate: %p\n", default_luactx));
    lua_close(default_luactx);
  }
  DBG_CTX(fprintf(stderr, "LUA: cleanup done\n"));
}

void
ase_lua_script_engine::force_gc()
{
  callfromase_guard g(this);
  lua_State *luactx = g.get_current_luactx();
  lua_gc(luactx, LUA_GCCOLLECT, 0);
}

void
ase_lua_script_engine::dump_contexts()
{
  force_gc();
  cached_contexts_type& ctxs = cached_contexts;
  DBG_CTX(fprintf(stderr, "LUA: dump sc=%p cached_contexts=%p\n", this,
    &cached_contexts[0]));
  for (ase_size_type i = 0; i < ctxs.size(); ++i) {
    DBG_CTX(lua_State *lc = ctxs[i]);
    DBG_CTX(fprintf(stderr, "LUA: dump ctx %p stacksize=%d\n", lc,
      lua_gettop(lc)));
  }
}

void
ase_lua_script_engine::dispose_proxies()
{
  DBG(fprintf(stderr, "LUA: dispose_proxies %p\n", this));
  DBG_CTX(dump_contexts());
  if (!default_luactx) {
    DBG(fprintf(stderr, "lua: dispose: null luactx\n"));
    return;
  }
  if (lua_endnode) {
    ase_lua_ase_variant_implnode *p = lua_endnode;
    do {
      DBG(fprintf(stderr, "disposeprxs gettop=%d\n",
        lua_gettop(default_luactx)));
      p->value.dispose(this, default_luactx);
      p = p->get_next();
    } while (p != lua_endnode);
  }
  if (ase_endnode) {
    ase_lua_aseproxy_privnode *p = ase_endnode;
    do {
      p->value.dispose(this);
      p = p->get_next();
    } while (p != ase_endnode);
  }
  DBG(fprintf(stderr, "LUA: dispose_proxies done\n"));
}

const char *const *
ase_lua_script_engine::get_global_names()
{
  static const char *const globals[] = { "Lua", 0 };
  return globals;
}

ase_variant
ase_lua_script_engine::get_global_object(const char *name)
{
  return luaglobal;
}

void
ase_lua_script_engine::import_ase_global(const ase_variant& val)
{
  callfromase_guard g(this);
  lua_State *luactx = g.get_current_luactx();
  DBG_IMPORT(fprintf(stderr, "LUA: importglobal %s\n", name));
  DBG_STACK(ase_lua_util::dump_gettop(luactx, "setgl1"));
  lua_pushstring(luactx, "ASE");
  ase_lua_util::to_lua_value(this, luactx, val);
  lua_settable(luactx, LUA_GLOBALSINDEX);
  DBG_STACK(ase_lua_util::dump_gettop(luactx, "setgl2"));
}

ase_variant
ase_lua_script_engine::load_file(const char *glname, const char *filename)
{
  ase_string fn = ase_string(filename);
  const ase_variant filebody = ase_load_file(fn).to_variant();
  return luaglobal.minvoke("EvalString", filebody, fn.to_variant());
}

ase_script_engine *
ase_lua_script_engine::get_real_engine()
{
  return this;
}

ase_lua_script_engine::callfromase_guard::callfromase_guard(
  ase_lua_script_engine *sc)
  : script_engine(sc), luactx(0)
{
  script_engine->uemutex.lock();
  {
    cached_contexts_type& ctxs = script_engine->cached_contexts;
    if (!ctxs.empty()) {
      /* use cached */
      luactx = ctxs.back();
      ctxs.pop_back();
      DBG_CTX(fprintf(stderr, "LUA: reusethread %p stacksize=%d\n", luactx,
        lua_gettop(luactx)));
    } else {
      /* create */
      lua_State *defctx = script_engine->default_luactx;
      DBG_CTX(fprintf(stderr, "LUA: top=%d\n", lua_gettop(defctx)));
      luactx = lua_newthread(defctx);
      lua_pushvalue(defctx, -1);
      lua_settable(defctx, LUA_REGISTRYINDEX); /* use luactx */
      DBG_CTX(fprintf(stderr, "LUA: newthread: %p top=%d\n", luactx,
	lua_gettop(defctx)));
    }
  }
}

ase_lua_script_engine::callfromase_guard::~callfromase_guard() /* DCHK */
{
  bool ctx_saved = false;
  try {
    lua_settop(luactx, 0);
    cached_contexts_type& ctxs = script_engine->cached_contexts;
    ctxs.push_back(luactx);
    DBG_CTX(fprintf(stderr, "LUA: cachethread: %p stacksize=%d\n", luactx,
      lua_gettop(luactx)));
    ctx_saved = true;
  } catch (...) {
  }
  if (!ctx_saved) {
    lua_pushthread(luactx);
    lua_pushnil(luactx);
    lua_settable(luactx, LUA_REGISTRYINDEX); /* unuse luactx */
    DBG_CTX(fprintf(stderr, "LUA: closethread: %p top=%d\n", luactx,
      lua_gettop(script_engine->default_luactx)));
  }
  script_engine->uemutex.unlock();
}

ase_lua_script_engine::callfromlua_guard::callfromlua_guard(
  const ase_variant& v, ase_lua_script_engine *sc)
  : unlock_flag((v.get_vtoption() & ase_vtoption_multithreaded) != 0),
    script_engine(sc)
{
  DBG_CTX(fprintf(stderr, "LUA: cfl begin\n"));
  DBG_CTX(script_engine->dump_contexts());
  if (unlock_flag) {
    script_engine->uemutex.unlock();
  }
}

ase_lua_script_engine::callfromlua_guard::~callfromlua_guard()
{
  DBG_CTX(fprintf(stderr, "LUA: cfl end\n"));
  DBG_CTX(script_engine->dump_contexts());
  if (unlock_flag) {
    script_engine->uemutex.lock();
  }
}

}; // namespace aselua

