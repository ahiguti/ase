
dofile("javadynload.lua")

local loader = GetFileLoader("ASETestJava.jar")
local klass = loader.loadClass("ASETestJava.TestObj")
local obj = klass()
obj.SetInt(3)
for i = 1, 10000 do
  obj.AddStrlen("Foobarbaz")
end
print(obj.GetInt())

