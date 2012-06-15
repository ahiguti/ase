
var TestVariant = GCJ["asejava.TestVariant"];

var callback_count = 0;

function Foo(x) {
  this.callback = function(y) {
    // print("callback: x=" + x);
    ++callback_count;
    return x + y;
  }
}

var o = new Foo(10);
o.callback(20);

var tvobj = TestVariant.NewInstance();
var baz = tvobj.baz;
for (var i = 0; i < 10000; ++i) {
 for (var j = 0; j < 1000; ++j) {
  var foo = new Foo(99);
  var r = baz(foo.callback, 10);
 }
 gc();
 print("done " + i);
}
print("baz r=" + r);
print("callback_count=" + callback_count);
