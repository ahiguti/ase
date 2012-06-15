
Perl.Use("TieTest");
var fs = Perl.Functions["TieTest"];
var x = fs.tietest(1);
x['a'] = 5;
x['b'] = 6;
x['c'] = 7;
print("xb=" + x['b']);
delete x['c'];
for (var i in x) {
  print("k=" + i + " v=" + x[i]);
}
