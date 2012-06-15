
local Test = Native["Util.Test"];
local i, r = 0
for i = 1, 10 do
  r = Test.Sum(10, 20, 30)
end
print(r)

