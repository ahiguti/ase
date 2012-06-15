
var JavaMath = Java["java.lang.Math"];

var x = 15.9;
var r = 0;
for (var i = 0; i < 10000000; ++i) {
  r = JavaMath.floor(x);
}
print(r);

