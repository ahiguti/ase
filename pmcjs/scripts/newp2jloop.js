
use('Perl');

Perl.Use("NewP2J");
nt = Perl.Functions["NewP2J"];

var r = 0;
for (var i = 0; i < 100000; ++i) {
  r = nt.newtest();
}
print(r);

