
Lua.dofile("js2lua.lua");
var tbl = Lua.newtable()
tbl["abc"] = 100;
tbl["def"] = 200;
print("[abc]=" + tbl["abc"]);
print("[def]=" + tbl["def"]);
delete tbl["abc"];
print("[abc]=" + tbl["abc"]);
tbl[35] = 350;
print("[35]=" + tbl[35]);

