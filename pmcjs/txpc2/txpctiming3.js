
var x = Components.classes["txpc"].createInstance().ITXPC;
var cbcount = 0;
var cb = {
  callback : function() { ++cbcount; }
};
x.doCallback(cb, 1000000);
print(cbcount);

