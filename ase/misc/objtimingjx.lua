
local Integer = Java["java.lang.Integer"]
local r, i
for i = 0, 1000000 do
  r = Integer("199")
end
print(r.intValue())

