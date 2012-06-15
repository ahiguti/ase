
local Remote = ASE.CPP.Remote("inet:localhost:10000");
print(Remote);
Remote.Perl.Use("IO::Dir");
local d = Remote.Perl.NewInstance("IO::Dir", ".")
while true do
  local n = d.read()
  if not n then break end
  print("ent: " .. n)
end

