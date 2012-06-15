
var x = Components.classes["txpc"].createInstance();
var cb = x.createCallback(0);
x.doCallback(cb, 1000000);
print("done");
