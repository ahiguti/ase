
var RObject = Rhino['Object'];
var RArray = Rhino['Array'];

var robj  = new RObject();
robj['foo'] = 123;
robj['bar'] = 45.98;
print(robj['foo']);
print(robj['bar']);

var rarr = new RArray();
for (var i = 0; i < 10; ++i) {
  rarr[i] = i * 2;
}
for (var i = 0; i < 10; ++i) {
  print(rarr[i]);
}

