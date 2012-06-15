
use('Perl');
Perl.Use('Class::Struct');
var struct = Perl.Functions['Class::Struct'].struct;
struct('MyStruct', Perl.NewHash({
  s: '$',
  a: '@',
  h: '%'
}));

var obj = new Perl('MyStruct');
obj.s(100);
print(obj.s());
obj.a(15, 9);
print(obj.a(15));
obj.h('abc', 'fubar');
print(obj.h('abc'));

