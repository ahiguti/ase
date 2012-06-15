
/* calling built-in function */

use('Perl');
var x = Perl.Eval("sub { return abs(shift); }");
var r = x(-17);
print(r);

