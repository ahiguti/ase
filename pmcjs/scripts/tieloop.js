
use('Perl');

Perl.Use("TieTest");
var fs = Perl.Functions["TieTest"];
var x = fs.tietest();
for (var i = 0; i < 1000000; ++i) {
  x['a'] = 5;
  x['b'] = 6;
  x['c'] = 7;
  r = x['b'];
  // delete x['c'];
}
print(r);

