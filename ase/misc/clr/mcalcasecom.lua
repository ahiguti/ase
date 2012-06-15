
local x = COM.CreateObject("MCalcServer.MCalc");
local i
for i = 1, 1000000 do
  x.Add(1)
end
print(x.Value);

