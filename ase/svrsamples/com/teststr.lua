
local obj = COM.CreateObject('ASETestCOM.TestObj')
obj.SetInt(3)
local i, r
for i = 1, 1000000 do
  obj.AddStrlen("Foobarbaz")
end
print(obj.GetInt)

