
local System = CLR.GetCore();
local x = CLR.CreateWrapper(System.System.Boolean, 0);

print(x);
print(x.Value);

x.Value = true;
print(x.Value);

