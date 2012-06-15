
var JObject = Java["java.lang.Object"];
var JInteger = Java["java.lang.Integer"];

var len = 20;
var arr = JObject.NewArray(len);
for (var i = 0; i < len; ++i) {
//  arr[i] = JInteger(i);
  arr[i] = "abc";
  print("i=" + i + " v=" + arr[i]);
}

var r = 0;
//for (var i = 0; i < 100; ++i) {
for (var i = 0; i < 1000000; ++i) {
  r = arr[3];
}
print("r=" + r);

