
local Test = ASE.CPP['ASETestCPP']
local arr = { 10, "foobar", 99.99999 }
local s = Test.ToDER(arr)
print(Test.ToHEX(s))
local v = Test.FromDER(s);
for i = 0, v.Length - 1 do
  print("v " .. i .. " " .. v[i])
end
