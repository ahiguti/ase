
local Test = {};
ASELuaUtil.Import(Test, ASE.CPP['ASETestCPP'], '')
local obj = Test.Create()
Test.SetInt(obj, 3)
local i, r
for i = 1, 1000000 do
  Test.AddInt(obj, 1)
end
print(Test.GetInt(obj))

