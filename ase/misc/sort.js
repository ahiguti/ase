
var Comparable = GCJ["java.lang.Comparable"];
var Arrays = GCJ["java.util.Arrays"];

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

var len = 20;
var arr = Comparable.NewArray(len);
for (var i = 0; i < len; ++i) {
  arr[i] = new Foo(Math.round(Math.random() * 1000));
  print("i=" + i + " v=" + arr[i].value);
}

Arrays.sort(arr);

for (var i = 0; i < len; ++i) {
  print("i=" + i + " v=" + arr[i].value);
}

