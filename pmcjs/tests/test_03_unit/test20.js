
/* tied array + method */

Perl.Use("TieTest");
var fs = Perl.Functions["TieTest"];
var ms = Perl.Methods["TieTest"];
var x = ms.newat(0);
x[3] = 5;
x[1] = 6;
x[9] = 7;
//x[11] = 7;
print("x1=" + x[1]);
print("enum1");
for (var i in x) {
  print("ITER k=" + i + " v=" + x[i]);
}
delete x[9];
print("enum2");
for (var i in x) {
  print("ITER k=" + i + " v=" + x[i]);
}
print("enum3");
for (var i = 0; i < x.length; ++i) {
  print("ROOP i=" + i + " v=" + x[i]);
}
var r = x.foo(15);
print(r);

