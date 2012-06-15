
local CT = Native.ComponentTest;
local obj, i, r
obj = CT()
for i = 0, 10000000 do
  r = obj.abs(-10)
end
print(r)

