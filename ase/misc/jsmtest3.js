
var lib = Native.LibTest;
var foo = lib.foo;
for (var i = 0; i < 10000000; ++i) {
  // var r = lib.foo();
  var r = lib.foo(3, 3);
}
print(r);
