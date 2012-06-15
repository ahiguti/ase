
var Integer = Java['java.lang.Integer'];

var x = Integer(100);
var y = Integer(100);
print(x.intValue());
print(y.intValue());
var eq = false;

/* timing */
for (var i = 0; i < 10000000; ++i) {
  eq = x.equals(y);
}

print("EQUALS? " + eq);

