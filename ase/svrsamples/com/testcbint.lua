
local obj = COM.CreateObject('ASETestCOM.TestObj')
local cb = {
  CallInt = function(x, y, z) 
    -- print("x=" .. x .. " y=" .. y .. " z=" .. z)
    return z
  end
}
obj.SetCallback(cb)
local r = obj.CallbackInt(1000000, 7, 8, 9);
print("result=" .. r)

