
local args = ASE.Host.Args
if (args[0] == nil) then
  error("Usage: cli.lua HOST");
end

local Remote = ASE.CPP.Remote(args[0]);
local o = Remote();
print(o.foo());
print(o.bar());
print("done");

