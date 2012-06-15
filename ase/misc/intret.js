
var JIntArray = Java['[I'];

// var arr = new Array(5);
var arr = JIntArray.NewInstance(5);
arr[0] = 100;
arr[1] = 200;
arr[2] = 300;
print(arr[1]);

var r = 0;
for (var i = 0; i < 1000000; ++i) {
  r = arr[1];
}
print("r=" + r);

