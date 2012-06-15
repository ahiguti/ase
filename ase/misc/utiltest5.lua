
ASELuaUtil.Import(nil, Native['Util.Test'], "UtilTest_");
local i, r = 0
for i = 1, 10000000 do
  r = UtilTest_Sum(10, 20, 30)
end
print(r)

