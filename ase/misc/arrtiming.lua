
local arr = {}
local i, r
for i = 0, 999 do
  arr[i] = i
end
r = 1
for i = 1, 10000000 do
  r = arr[0]
end
print("r=" .. r);

