
var lib = Native.LibTest;
var foo = lib.foo;
for (var i = 0; i < 100; ++i) {
  // r = lib.abc(3, 5);
  var r = lib.foo(3, 5);
  // r = foo(3, 5);
//  if ((i & 4095) == 0) {
//    gc();
//  }
}
print(r);
