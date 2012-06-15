
var x = Components.classes["txpc"].createInstance();

x.setValue(100);
print("getValue: " + x.getValue());

var cb = {
  callback : function() { print("JS function called"); }
};
x.doCallback(cb, 3);

var cb2count = 0;
var cb2 = {
  callback : function() { ++cb2count; }
};
x.doCallback(cb2, 9999);
print("cb2count: " + cb2count);

var cppcb = x.createCallback(true);
cppcb.callback();

