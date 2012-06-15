
use('Perl');

var x = Perl.NewHash();
for (var i = 0; i < 10000000; ++i) {
  x["abc"] = 3;
}
print(x["abc"]);

