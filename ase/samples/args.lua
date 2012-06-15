
local args = ASE.Host.Args
print("nargs=" .. args.Length);
local i
for i = 0, args.Length - 1 do
  print("arg " .. i .. " " .. args[i]);
end

