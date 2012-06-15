
use('Perl');
Perl.Use('MethodTest');
var o = new Perl('MethodTest');
for (var i = 0; i < 100; ++i) {
  o.foo();
}
print(o.foo());

