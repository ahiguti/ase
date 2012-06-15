
var obj = new ActiveXObject('ATLDispTest.MITest');
var o3 = new ActiveXObject('ATLDispTest.MITest');
for (var i = 0; i < 1000000; ++i) {
  x = obj.Foo(o3);
}
WScript.Echo("x=" + x);
