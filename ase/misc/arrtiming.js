
var arr = [];
for (var i = 0; i < 1000; ++i) {
  arr[i] = i;
}
var r = 1;
for (var i = 0; i < 10000000; ++i) {
  r = arr[0];
}
print("r=" + r);

