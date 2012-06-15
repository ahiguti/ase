
var RObject = Rhino['Object'];
var RArray = Rhino['Array'];

var robj  = RObject();
robj['foo'] = 123;
robj['bar'] = 45.98;
robj['baz'] = 9999;
for (var i in robj) {
  print(i + " : " + robj[i]);
}
null;
