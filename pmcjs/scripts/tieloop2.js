
use('Perl');

Perl.Use("TieTest");
var fs = Perl.Functions["TieTest"];
for (var i = 0; i < 1000000; ++i) {
  var x = Perl.NewHash();
  var y = Perl.Tie(x, 'TieTest::HTie', 0);
  x['a'] = 5;
  x['b'] = 6;
  x['c'] = 7;
  r = x['b'];
  // delete x['c'];
}
print(r);

