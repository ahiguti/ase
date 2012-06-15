
local Test = Native.Util.Test
local i, r = 0
local Sum = Test.Sum
local x = {}
x.Sum = Test.Sum
for i = 1, 10000000 do
  -- r = Sum(1, 10)
  r = Sum(Native, 10)
end
print(r)

