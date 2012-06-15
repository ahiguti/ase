
local args = Host.Args
print("nargs=" .. args.length);
local i
for i = 0, args.length - 1 do
  print("arg " .. i .. " " .. args[i]);
end

