
local Test = {};
ASELuaUtil.Import(Test, Native['ASETestNative.TestNative'], '')
local obj = Test.NewInstance()
Test.SetInt(obj, 3)
local i, r
for i = 1, 10000000 do
  Test.AddInt(obj, 1)
end
print(Test.GetInt(obj))

