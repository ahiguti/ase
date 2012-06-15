
use('Perl');
Perl.Use('MethodTiming2');


for (var i = 0; i < 1000000; ++i) {
  var x = new Perl('MethodTiming2');
  var r = x.foo();
}

print(r);

