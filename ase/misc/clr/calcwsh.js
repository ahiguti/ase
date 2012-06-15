
var x = new ActiveXObject("CalcServer.Calc");
for (var i = 0; i < 1000000; ++i) {
  x.add(1);
}
WScript.Echo(x.value());

