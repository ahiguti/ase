
/* SKIP_CHECKMEM */

use('Perl');

var f = Perl.Eval('sub { }');
Perl.Eval('sub foo { }');
var f2 = Perl.Functions[''].foo;

for (var i = 0; i < 1000000; ++i) {
  f();
}

