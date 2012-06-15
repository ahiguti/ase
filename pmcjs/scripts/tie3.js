
use('Perl');

Perl.Use("TieTest");
var x = Perl.NewTiedHashRef("TieTest::HTie", 1, "foo", "bar");
x['abc'] = 5;
x['xyz'] = 6;
var y = Perl.NewTiedArrayRef("TieTest::ATie", 1, "abc");
y[3] = 7;
var z = Perl.NewTiedScalar("TieTest::STie", 1, 0, 0);
print("z=" + z.Get());
z.Set(5);
print("z=" + z.Get());

