
var Integer = Java['java.lang.Integer'];

var arr = Integer.NewArray(5);
var x = Integer(100);
var y = Integer(200);
print(x.intValue());
print(y.intValue());
arr[0] = x;
arr[1] = y;
arr[2] = x;

var r = 0;
for (var i = 0; i < 1000000; ++i) {
  r = arr[1];
}
print("r=" + r.intValue());

