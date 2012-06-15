
use('Perl');
Perl.Use('Data::Dumper');

var x = {
  foo: 'bar',
  baz: 'fubar'
};
var sx = Perl.Functions['Data::Dumper'].Dumper(x);
print(sx);

var y = Perl.NewHash(x);
var sy = Perl.Functions['Data::Dumper'].Dumper(y);
print(sy);
