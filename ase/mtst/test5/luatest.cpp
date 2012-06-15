
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

// extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
// }

#include <string.h>

#define DBG_INVOKE(x)
#define DBG_PROP(x)

static int calcsum(lua_State *lua)
{
  int n = lua_gettop(lua);
  lua_Number r = 0;
  for (int i = 1; i <= n; ++i) {
    double x = lua_tonumber(lua, i);
    printf("x=%lf\n", x);
    r += x;
  }
  lua_pushnumber(lua, r);
  return 1;
}

static int
invokegeneric2(lua_State *lua)
{
  int n = lua_gettop(lua);
  void *thisptr = lua_touserdata(lua, lua_upvalueindex(1));
  (void)thisptr;
  const char *s = lua_tostring(lua, lua_upvalueindex(2));
  int len = strlen(s);
  DBG_INVOKE(printf("invokegeneric: gettop=%d name=%s\n", n, s));
  if (n < 1) {
    return 0;
  }
  double x = lua_tonumber(lua, 1);
  lua_pushnumber(lua, x * 2 + len);
  return 1;
}

static int
getpropgeneric2(lua_State *lua)
{
  int n = lua_gettop(lua);
  DBG_PROP(printf("getpropgeneric: gettop=%d\n", n));
  if (n < 2) {
    return 0;
  }
  /* enclose 'this' and 'name' */
  lua_pushvalue(lua, 1);
  lua_pushvalue(lua, 2);
  lua_pushcclosure(lua, invokegeneric2, 2);
  return 1;
}

static int
invokegeneric1(lua_State *lua)
{
  int n = lua_gettop(lua);
  const char *s = lua_tostring(lua, lua_upvalueindex(1));
  int len = strlen(s);
  DBG_INVOKE(printf("invokegeneric: gettop=%d name=%s\n", n, s));
  if (n < 1) {
    return 0;
  }
  double x = lua_tonumber(lua, 1);
  lua_pushnumber(lua, x * 2 + len);
  return 1;
}

static int
getpropgeneric1(lua_State *lua)
{
  int n = lua_gettop(lua);
  DBG_PROP(printf("getpropgeneric: gettop=%d\n", n));
  if (n < 2) {
    return 0;
  }
  /* enclose 'name' */
  lua_pushvalue(lua, 2);
  lua_pushcclosure(lua, invokegeneric1, 1);
  return 1;
}

int
main(int argc, char **argv)
{
  if (argc < 2) {
    fprintf(stderr, "%s FILENAME.lua\n", argv[0]);
    return 0;
  }
  lua_State *lua = lua_open();
  luaopen_base(lua);

  lua_register(lua, "calcsum", calcsum);

  /* metatable for ud1 */
  {
    printf("x: gettop=%d\n", lua_gettop(lua));
    lua_pushstring(lua, "ud1");   // +1
    void *udata = lua_newuserdata(lua, 8); // +1
    printf("udata=%p\n", udata);
    lua_newtable(lua);           // +1
    /* { "ud1", udata, mtable } */
    {
      lua_pushstring(lua, "__index");  // +1
      lua_pushcfunction(lua, getpropgeneric1); // +1
      /* { "ud1", udata, mtable, "__index", getpropgeneric } */
      lua_settable(lua, -3); /* mtable.__index = getpropgeneric */ // -2
    }
    lua_setmetatable(lua, -2); // -1
    lua_settable(lua, LUA_GLOBALSINDEX); // -2
    printf("y: gettop=%d\n", lua_gettop(lua));
  }
  /* metatable for ud2 */
  {
    printf("x: gettop=%d\n", lua_gettop(lua));
    lua_pushstring(lua, "ud2");   // +1
    void *udata = lua_newuserdata(lua, 8); // +1
    printf("udata=%p\n", udata);
    lua_newtable(lua);           // +1
    /* { "ud2", udata, mtable } */
    {
      lua_pushstring(lua, "__index");  // +1
      lua_pushcfunction(lua, getpropgeneric2); // +1
      /* { "ud2", udata, mtable, "__index", getpropgeneric } */
      lua_settable(lua, -3); /* mtable.__index = getpropgeneric */ // -2
    }
    lua_setmetatable(lua, -2); // -1
    lua_settable(lua, LUA_GLOBALSINDEX); // -2
    printf("y: gettop=%d\n", lua_gettop(lua));
  }

  lua_register(lua, "getpropgeneric1", getpropgeneric1);
  lua_register(lua, "getpropgeneric2", getpropgeneric2);


  luaL_dofile(lua, argv[1]);

#if 0
  lua_pushstring(lua, "add");
  lua_gettable(lua, LUA_GLOBALSINDEX);
  lua_pushnumber(lua, 100);
  lua_pushnumber(lua, 200);
  lua_call(lua, 2, 1);
  double r = lua_tonumber(lua, 1);

  printf("result=%lf\n", r);
#endif
  return 0;
}

