
var JDate = Java['java.util.Date'];
var JDateArr = Java['[Ljava.util.Date;'];
var arr = JDateArr(10);
for (var i = 0; i < 10; ++i) {
  arr[i] = JDate();
}
var r = 0;
for (var i = 0; i < 1000000; ++i) {
  r = arr[0];
}
print(r.toGMTString());

