
use('Perl');
Perl.Use("Math::BigInt");

var x = new Perl("Math::BigInt", "100");
x.bfac();
print(x.bstr());

