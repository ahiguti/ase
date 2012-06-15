
var testv = GCJ["asejava.TestVariant"].NewInstance();
var f = testv.bar;
for (var i = 0; i < 10000000; ++i) {
  var r = testv.bar(10.5)
}
print(r);

