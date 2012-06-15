
var x = new ActiveXObject("MCalcServer.MCalc");
for (var i = 0; i < 1000000; ++i) {
  x.Add(1);
}
WScript.Echo(x.Value());

