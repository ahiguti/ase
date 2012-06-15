
use('Perl');
var z;
for (var lp = 0; lp < 1000; ++lp) {
  var x = Perl.NewHash();
  for (var i = 0; i < 1000; ++i) {
    x[i] = i;
  }
  z = x[999];
}
print(z);

