
use('Perl');
Perl.Use('MethodTiming');

var x = new Perl('MethodTiming');

var r = 0;
for (var i = 0; i < 1000000; ++i) {
  r = x.foo();
}

print(r);

