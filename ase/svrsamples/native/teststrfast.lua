
local Test = {};
ASELuaUtil.Import(Test, Native['ASETestNative.TestNative'], '')
local obj = Test.NewInstance()
Test.SetInt(obj, 3)
local i, r
for i = 1, 1000000 do
  Test.AddStrlen(obj, "Foobarbaz")
end
print(Test.GetInt(obj))

