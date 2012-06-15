
use('Perl');

var x;
for (var i = 0; i < 10000000; ++i) {
  x = Perl.NewScalar();
}
print("done");

