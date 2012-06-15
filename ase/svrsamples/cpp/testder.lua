
local Test = ASE.CPP['ASETestCPP']
local o = { 10, 20, 30 }
for i = 1, 100000 do
  local s = Test.ToDER(o)
end

