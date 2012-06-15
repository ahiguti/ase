
local obj, i, r
obj = Native.ComponentTest()
for i = 0, 1000 do
  obj = Native.ComponentTest()
  r = obj.abs(-10)
end
print(r)

