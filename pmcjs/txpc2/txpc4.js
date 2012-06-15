
var x = Components.classes["txpc"].createInstance();

var cb = x.createCallback(1);
x.saveCallback(cb, 1);
x.doSavedCallback(3);
cb = null;
gc();
x.doSavedCallback(3); /* should fail */

