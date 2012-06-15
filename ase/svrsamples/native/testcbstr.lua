
local Test = Native['ASETestNative.TestNative']
local obj = Test()
local cb = function(x, y, z) 
  -- print(x)
  return z
end
obj.SetCallback(cb)
local r = obj.CallbackString(1000000, "foo", "bar", "baz");
print(r)

