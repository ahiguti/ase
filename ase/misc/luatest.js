
Lua.dofile("js2lua3.lua");
var obj = Lua.luafunc()
obj(); /* should cause an error: attempt to call a table value */

