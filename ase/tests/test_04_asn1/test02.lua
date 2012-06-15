
local Test = ASE.CPP['ASETestCPP']
local arr = { 10, "foobar", 99.99999 }
local s = Test.ToXML(arr)
local v = Test.FromXML(s);
for i = 0, v.Length - 1 do
  print("v " .. i .. " " .. v[i])
end
