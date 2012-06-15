
use('Perl');
var x = Perl.Eval("sub { return cos(shift); }");
var r = x(3.14159);
print(r);

