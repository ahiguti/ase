
var Comparable = Java["java.lang.Comparable"];

function Foo(value) {
  this.value = value;
  this.compareTo = function(x) {
    var xv = x.value;
    print("compareTo " + value + " " + xv);
    if (value < xv) { return -1; }
    if (value > xv) { return 1; }
    return 0;
  };
}

var r = 0;
for (var i = 0; i < 100000; ++i) {
  r = Comparable(new Foo(100));
}

print("v=" + r.value);

