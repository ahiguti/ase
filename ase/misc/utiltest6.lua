
local Test = {}
ASELuaUtil.Import(Test, Native['Util.Test'], '');
local obj = Test.NewInstance()
Test.SetValue(obj, 100)
print(Test.GetValue(obj))
local i, r = 0
for i = 1, 10000000 do
  Test.Add(obj, 1)
end
print(Test.GetValue(obj))
r = Test.Sum(10, 20, 30)
print(r)

