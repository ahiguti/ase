
use('Perl');

var x = Perl.NewScalar();
var z = "";
x.Set("abc");
for (var i = 0; i < 10000000; ++i) {
  x.Set("xyz");
  z = x.Get("xyz");
}
print(z);

