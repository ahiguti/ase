
local lib, i, r
print("AAAAA")
lib = Native.LibTest
print(lib)
print("BBBBB")
for i = 0, 10 do
  r = lib.abs(-10)
end
print("CCCCC")
print(r)

