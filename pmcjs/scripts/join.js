
use('Perl');
var join = Perl.Eval("sub { my ($x, $y) = @_; return join $x, @$y; }");
var arr = [ 'foo', 'bar', 'baz' ];
var x = join(':', arr);
print(x);

