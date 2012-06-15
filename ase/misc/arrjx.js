
var IArr = Java['[I'];
var arr = IArr(10);
print(arr);
for (var i = 0; i < 10; ++i) {
  print("wr " + i);
  arr[i] = i * i;
  print("wr done");
}
for (var i = 0; i < 10; ++i) {
  print(arr[i]);
}
