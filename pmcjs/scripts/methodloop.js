
use('Perl');
Perl.Use('MethodTiming');


for (var i = 0; i < 1000000; ++i) {
  var x = new Perl('MethodTiming');
  var r = x.foo();
}

print(r);

