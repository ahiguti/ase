
dofile('clrdynload.lua')
local kl = LoadDll('asetestclr.dll', 'ASETestCLR.TestObj')
local de = LoadDll('asetestclr.dll', 'ASETestCLR.CallbackDelegate')
local obj = kl()
obj.SetInt(3)
local cb = function(x, y, z) 
  -- print("x=" .. x .. " y=" .. y .. " z=" .. z)
  return z
end
obj.SetCallback(CLR.CreateDelegate(de, cb))
local r = obj.CallbackInt(1000000, 7, 8, 9);
print("result=" .. r)

