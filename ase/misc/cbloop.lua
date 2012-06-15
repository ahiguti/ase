
function cb(x)
  return x + x
end

local r = Native.LibTest.loop(cb, 10, 1000000);
print(r);

