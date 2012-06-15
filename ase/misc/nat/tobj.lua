
local NTestMod = Native.Util.Meta.LoadFile("ntest.cpp")
local TestNative = NTestMod.TestNative
local i = 0
for j = 1, 10000000 do 
  local obj = TestNative()
  obj.SetInt(j)
  i = j;
end
print(i)

