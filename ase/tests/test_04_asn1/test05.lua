
local Test = ASE.CPP['ASETestCPP']
local x = ""
for i = 1, 1000 do
  x = x .. "a"
  local s = Test.ToDER(x)
  local v = Test.DERLen(s)
  if (not v[0]) then
    print("failed")
  else
    if (v[2]) then
      print("ov")
    else
      print("len=" .. v[1] .. " derlen=" .. #s)
    end
  end
end
