
use('Perl');
Perl.Use("Math::BigInt");

var x = new Perl("Math::BigInt", "100");
var y = x.bfac();
print(x.bstr());

