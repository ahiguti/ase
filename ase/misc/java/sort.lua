
local Lang = Java.java.lang;
local Comparable = Lang.Comparable;
local Util = Java.java.util;
local Arrays = Util.Arrays;

function Foo(value)
  local o = {}
  o.value = value
  o.compareTo = function(x)
    local xv = x.value
    print("compareTo: " .. value .. " " .. xv)
    if (value < xv) then return -1 end
    if (value > xv) then return 1 end
    return 0
  end
  return o
end

math.randomseed(os.time())

local len = 20
local arr = Comparable.NewArray(len)
local i
for i = 0, len - 1 do
  arr[i] = Comparable(Foo(math.ceil(math.random() * 1000)))
  print("i=" .. i .. " v=" .. arr[i].value)
end

Arrays.sort(arr)

for i = 0, len - 1 do
  print("i=" .. i .. " v=" .. arr[i].value)
end

