
function cb(x, y, z)
  print("x=" .. x .. " y=" .. y .. " z=" .. z)
  return x + y + z
end

local r = Native['Util.Test'].CallbackTest(cb, 10, 22, 33)
print(r)

