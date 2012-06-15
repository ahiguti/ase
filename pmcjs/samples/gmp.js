
use('Perl');
Perl.Use('Math::GMP');
var x = new Perl('Math::GMP', 100);
x = x.bfac();
print(x.stringify());

