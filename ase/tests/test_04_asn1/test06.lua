
local Test = ASE.CPP['ASETestCPP']
-- local arr = { 10, "foobar", 99.99999 }
-- local v = 99.99999
local s = Test.ToSerial(10, "foobar", 99.99999)
print(Test.ToHEX(s))
print(#s)
local v = Test.FromSerial(s);
for i = 0, v.Length - 1 do
  print("v " .. i .. " " .. v[i])
end
local l = Test.SerialLen(s)
print(l[0])
print(l[1])
