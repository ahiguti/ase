
function add(x, y)
  return x + y
end

local t = {}
local m = { ["__index"] = getpropgeneric }
setmetatable(t, m)
local x = 0
local f = t.foo;
for i = 0, 10000000 do
  x = ud.bar(999)
  -- x = t.foo(100)
  -- x = f(100)
end
print(x)
local y = ud.bar(99);
print(y)
-- collectgarbage()
-- print(calcsum(1, 2, 3, 4, 5))
-- print(calcsum(10, 20))

