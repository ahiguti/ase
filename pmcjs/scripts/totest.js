
use('Perl');
Perl.Use('ToTest');
var x = new Perl('ToTest');
x.foo();
var r = x.toXString();
print(r);
var r = x.toLocaleString();
print(r);
