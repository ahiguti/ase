
/* tied hash + method */

Perl.Use("TieTest");
var fs = Perl.Functions["TieTest"];
var ms = Perl.Methods["TieTest"];
var x = ms.newht(0);
x['a'] = 5;
x['b'] = 6;
x['c'] = 7;
print("xb=" + x['b']);
delete x['c'];
for (var i in x) {
  print("k=" + i + " v=" + x[i]);
}
var r = x.foo(15);
print(r);

