
use('Perl');

var x = { v : 0 };
for (i = 0; i < 1000000; ++i) {
  x.v = 100;
}
print(x.v);
