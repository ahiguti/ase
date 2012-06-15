
local Test = {}
ASELuaUtil.Import(Test, Native.Util.Test, '');
local i, r = 0
local obj
for i = 1, 10000000 do
  obj = Test.NewInstance()
  Test.Add(obj, 1)
end
print(Test.GetValue(obj))
r = Test.Sum(10, 20, 30)
print(r)

