
var TLB = Java['asejni.TestLateBinding'];

var x = TLB(3, "FOO");
// var y = x.getProperty("abc");
// var y = x.abc;
x.abc = "setval0";
// print(y);

