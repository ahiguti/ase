
var x = COM.CreateObject("CalcServer.Calc");
print(x);
for (var i = 0; i < 1000000; ++i) {
  x.add(1);
}
print(x.value);

