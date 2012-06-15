
var lib = Native.LibTest;
var foo = lib.foo;

var x = newjsmt();
for (var i = 0; i < 10000000; ++i) {
  var r = x.func1();
}
print(r);
