
use('Perl');
var x = 0;
for (var i = 0; i < 10000000; ++i) {
  Perl.ERRNO = 1;
  x = Perl.ERRNO;
}
print(x);
