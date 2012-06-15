
local obj = COM.CreateObject('ASETestCOM.TestObj')
local cb = {
  CallString = function(x, y, z) 
    -- print("x=" .. x .. " y=" .. y .. " z=" .. z)
    return z
  end
}
obj.SetCallback(cb)
local r = obj.CallbackString(1000000, "foo", "bar", "baz");
print("result=" .. r)

