
local Test = ASE.CPP['ASETestCPP']
local obj = Test.Create()
obj.SetInt(3)
local i, r
for i = 1, 100000 do
  obj.AddInt(1)
end
print(obj.GetInt())

