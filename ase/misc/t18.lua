
local UUID = Native.Util.UUID
local i
for i = 1, 10 do
  local x = UUID(1111, 1, 2)
  print(x.upper())
end

