
local x = COM.CreateObject("CalcServer.Calc");
local i
for i = 1, 1000000 do
  x("add", 1)
end
print(x.value);

