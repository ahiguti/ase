
local Test = Native['Util.Test']
local i, r = 0
local obj = Test()
local TestAdd      = ASELuaUtil.CreateUnbound(Test, "Add");
local TestGetValue = ASELuaUtil.CreateUnbound(Test, "GetValue");
local TestSetValue = ASELuaUtil.CreateUnbound(Test, "SetValue");
TestSetValue(obj, 100)
print(TestGetValue(obj))
for i = 1, 10000000 do
  TestAdd(obj, 1)
end
print(TestGetValue(obj))

