
local Test = ASE.CPP['ASETestCPP']
local obj = Test.Create()
obj.SetInt(5)
local r
r = obj.IFunc(1, 2, 3, 4)
print(r)
r = Test.SFunc(1, 2, 3, 4)
print(r)
