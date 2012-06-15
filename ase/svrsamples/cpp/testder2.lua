
local Test = ASE.CPP['ASETestCPP']
local o = { 10, 20, 30 }
local loop = 100000
local s = Test.ToDERLoop(o, loop)
print("loop=" .. loop)

