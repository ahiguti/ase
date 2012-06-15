
local NTestMod = Native.Util.Meta.LoadFile("ntest.cpp")
local TestNative = NTestMod.TestNative
local arr = {}
for j = 1, 1000000 do 
  local obj = TestNative()
  obj.SetInt(j * 2)
  arr[j] = obj
end
local obj = arr[23];
print(obj.GetInt())

-- dummy loop
for j = 1, 1000000 do 
  obj = arr[j]
  local x = obj.GetInt()
  obj.SetInt(x * 3)
end
