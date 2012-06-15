
use('Perl');
Perl.Use("Math::BigInt");

var x = new Perl("Math::BigInt", "100");
var f = x.bfac;
f();
print(x.bstr());

