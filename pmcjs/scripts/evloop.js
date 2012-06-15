
use('Perl');
var x;
for (var i = 0; i < 1000000; ++i) {
  x = Perl.Eval(" 'abc'; ");
}
print(x);
