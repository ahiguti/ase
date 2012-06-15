
var c = Native.ComponentTest;
var x = c();
for (var i = 0; i < 3; ++i) {
  var r = x.foo(10);
  var r = x.bar(10);
  var r = x.baz(10);
  var r = c.hoge(10);
  var r = c.fuga(10);
}
print(r);

