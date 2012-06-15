
local Test = Native['ASETestNative.TestNative']
local obj = Test()
local cb = function(x, y, z) 
  -- print(x)
  return z
end
obj.SetCallback(cb)
local r = obj.CallbackInt(1000000, 7, 8, 9);
print(r)

