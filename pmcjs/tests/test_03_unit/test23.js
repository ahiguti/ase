
/* test for NewScalar(value) */

var f = Perl.Eval("sub { print \"@_[0]\\n\"; }");
var x = Perl.NewScalar("abc");
f(x);

