
use('Perl');

Perl.Use("TieTest");
var w;
for (var i = 0; i < 100000; ++i) {
  var z = Perl.NewTiedScalar("TieTest::STie", 0, 0, 0);
  w = z.Get();
  z.Set(5);
  w = z.Get();
}
print(w);

