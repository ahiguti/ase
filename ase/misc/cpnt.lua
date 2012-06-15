
local c = Native.ComponentTest
local x = c()
local i, r
for i = 0, 2 do
  r = x.foo(10)
  r = x.bar(10)
  r = x.baz(10)
  r = c.hoge(10)
  r = c.fuga(10)
end
print(r)

