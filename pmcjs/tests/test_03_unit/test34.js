
var f = Perl.Eval('sub { return $_[0] + $_[1] + $_[2]; }');
print(f(10, 20, 30));

var arr = [ 100, 200, 300 ];
print(Perl.Apply(f, arr));

