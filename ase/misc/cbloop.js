
function cb(x) {
  return x + x;
}

var r = Native.LibTest.loop(cb, 10, 1000000);
print(r);
