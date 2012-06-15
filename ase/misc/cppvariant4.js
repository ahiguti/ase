
var obj = Native.ComponentTest();
for (var i = 0; i < 10000000; ++i) {
  var r = obj.abs(-10);
}
print(r);

