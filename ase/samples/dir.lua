
ASE.Perl.Use("IO::Dir")
local d = ASE.Perl.NewInstance("IO::Dir", ".")
while true do
  local n = d.read()
  if not n then break end
  print("ent: " .. n)
end

