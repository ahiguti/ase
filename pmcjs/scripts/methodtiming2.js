
use('Perl');
Perl.Use('MethodTiming2');

var x = new Perl('MethodTiming2');

for (var i = 0; i < 1000000; ++i) {
  var r = x.foo();
}

print(r);

