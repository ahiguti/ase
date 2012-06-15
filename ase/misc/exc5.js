
var Comparable = ASE.Java["java.lang.Comparable"];
var Arrays = ASE.Java["java.util.Arrays"];

function Hoge() {
  this.toString = function(x) {
    return "hoge";
  }
}

function Foo(v) {
  var value = v;
  this.get = function(x) { return v; }
  this.compareTo = function(x) {
    var xv = x.get();
    print("compareTo " + value + " " + xv);
    throw new Hoge();
    if (value < xv) { return -1; }
    if (value > xv) { return 1; }
    return 0;
  };
}

var arr = Comparable.NewArray(5);
for (var i = 0; i < 5; ++i) {
  arr[i] = Comparable(new Foo(100 - i * i));
}

print("sort");
Arrays.sort(arr);
for (var i = 0; i < 5; ++i) {
  print("i=" + i + " v=" + arr[i].get());
}

