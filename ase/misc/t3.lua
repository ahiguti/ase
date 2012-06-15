
function cb(x)
  return x + x
end

local r = Native.LibTestx.loop(cb, 10, 1000000);
print(r);

