
local obj, i, r
obj = Native.ComponentTest.Test()
for i = 0, 10000000 do
  r = obj.abs(-10)
end
print(r)

