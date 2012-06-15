
local UUID = Native.Util.UUID
local i
for i = 1, 10 do
  local x = UUID()
  print(x.upper())
end

