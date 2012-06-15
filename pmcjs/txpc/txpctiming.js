
var x = Components.classes["txpc"].createInstance();
for (var i = 0; i < 1000000; ++i) {
  x.setValue(100);
}
print(x.getValue());

