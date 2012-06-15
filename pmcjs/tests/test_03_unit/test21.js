
/* tied scalar */

Perl.Use("TieTest");
var z = Perl.NewTiedScalar("TieTest::STie", 1, 0, 0);
print("z=" + z.Get());
z.Set(5);
print("z=" + z.Get());

