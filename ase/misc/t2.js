
var lib = Native.LibTest;
var foo = lib.foo;
for (var i = 0; i < 10000000; ++i) {
  var r = lib.foo();
}
print(r);
