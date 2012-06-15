
local Integer = Java["java.lang.Integer"]
local r, i
-- print("A")
for i = 0, 1000000 do
  r = Integer(199)
end
-- print("B")
print(r.intValue())

