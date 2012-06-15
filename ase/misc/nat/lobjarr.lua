
local NTestMod = Native.Util.Meta.LoadFile("ntest.cpp")
local TestNative = NTestMod.TestNative
local arr = {}
for j = 1, 1000000 do 
  local obj = { abc = 3 }
  arr[j] = obj
end
print("done")

-- dummy loop
local x = {}
--for j = 1, 100000000 do 
--  x[0] = j
--end
