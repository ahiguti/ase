
local TestNativeMod = Native.Util.Meta.LoadFile("testnative.cpp")
local TestNative = TestNativeMod.TestNative
local obj = TestNative()
obj.SetInt(3)
local i, r
for i = 1, 1000000 do
  obj.AddStrlen("Foobarbaz")
end
print(obj.GetInt())


