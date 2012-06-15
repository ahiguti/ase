
use('Perl');

Perl.Use("TieTest");
var fs = Perl.Functions["TieTest"];
var x = fs.tietest(1);
//var x = Perl.NewHash();
x['a'] = 5;
x['b'] = 6;
x['c'] = 7;
print("xb=" + x['b']);
try {
  delete x['c'];
} catch (e) {
  print("catch: " + e);
}
for (var i in x) {
  print("k=" + i + " v=" + x[i]);
}
print("done");
