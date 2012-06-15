
var Math = Java['java.lang.Math'];
var System = Java['java.lang.System'];
var floor = Math.floor;

var x = 15.9;
var r = 0;
// for (var i = 0; i < 1; ++i) {
for (var i = 0; i < 1000000; ++i) {
  for (var j = 0; j < 100000; ++j) {
    r = Math.floor(x);
  }
  gc();
  print(i);
  // System.gc();
  // var r = floor(x);
}
print(r);
