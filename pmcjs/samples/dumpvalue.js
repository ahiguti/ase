
use('Perl');
Perl.Use('Dumpvalue');
var dumper = new Perl('Dumpvalue');

var x = {
  foo: 'bar',
  baz: 'fubar',
  fubar: {
    a: 15,
    b: 20
  }
};
dumper.dumpValue(x);

var y = Perl.NewHash(x);
y.fubar = Perl.NewHash(x.fubar);
dumper.dumpValue(y);
