
dofile("javadynload.lua")

local loader = GetFileLoader("ASETestJava.jar")
local klass = loader.loadClass("ASETestJava.TestObj")
local cbklass = loader.loadClass("ASETestJava.CallbackInterface")
local obj = klass()
obj.SetInt(3)
local cb = {
  CallInt = function(x, y, z)
    -- print(x)
    return z
  end
}
obj.SetCallback(cbklass(cb))
local r = obj.CallbackInt(1000000, 7, 8, 9)
print(r)

