
local ass = CLR.LoadAssembly("C:/Program Files/CalcSetup/MCalcServer.dll");
local mcalc = ass.GetType("MCalcServer.MCalc");
local x = mcalc.NewInstance();
local i
for i = 1, 1000000 do
  x.Add(1)
end
print(x.Value);

