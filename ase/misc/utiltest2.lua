
local Test = Native.Util.Test
local i, r = 0
local obj = Test()
obj.SetValue(100)
print(obj.GetValue())
for i = 1, 10000000 do
  obj.Add(1)
end
print(obj.GetValue())

