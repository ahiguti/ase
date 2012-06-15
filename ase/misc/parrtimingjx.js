
var JIntArr = Java['[I'];
var arr = JIntArr(10);
for (var i = 0; i < 10; ++i) {
  arr[i] = i;
}
var r = 0;
for (var i = 0; i < 1000000; ++i) {
  r = arr[5];
}
print(r);

