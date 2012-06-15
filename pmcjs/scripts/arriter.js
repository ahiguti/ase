
var arr = [ 99, 98, 97 ];
arr[4] = 97;
arr[1] = 999;
arr[2] = 998;
arr[3] = 997;

/* 0, 1, 2, 4, 3 */
for (var i in arr) {
  print(i + ": " + arr[i]);
}
