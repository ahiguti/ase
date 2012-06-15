
local S = Native.Syntax
local e1 = S.Get('x', 'm1')
local e2 = S.Get('y', e1)
local e3 = S.Let('z', e2)
local m = S.Method("foo", nil, S.Args('x', 'y'), e3)
local k = S.Class("Bar", m);
print(k)

