
local lib, i, r
lib = Native.LibTest
for i = 0, 10000000 do
  r = lib.abs(-10)
end
print(r)

