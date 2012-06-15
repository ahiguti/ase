
use('Perl');
var z;
for (var lp = 0; lp < 1000; ++lp) {
  var x = Perl.NewArray();
  for (var i = 0; i < 1000; ++i) {
    x[i] = i;
  }
  z = x.length;
}
print(z);

