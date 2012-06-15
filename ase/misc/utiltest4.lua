
ASELuaUtil.Import(nil, Native['Util.Test'], "UtilTest_");
local obj = UtilTest_NewInstance()
UtilTest_SetValue(obj, 100)
print(UtilTest_GetValue(obj))
local i, r = 0
for i = 1, 10000000 do
  UtilTest_Add(obj, 1)
end
print(UtilTest_GetValue(obj))
r = UtilTest_Sum(10, 20, 30)
print(r)

