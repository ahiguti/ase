
local S = Native.Syntax
local x = S.Value(15)
local y = S.Value(22)
local m = S.Method("Foo", {'x', 'y'}, {}, nil)
print(m)

