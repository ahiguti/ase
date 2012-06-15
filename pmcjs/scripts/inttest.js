
use('Perl');

Perl.Use("IntTest");
var x = 1;
var pkg = Perl.Functions["IntTest"];
for (var i = 0; i < 40; ++i) {
  var y = pkg.add(x, x);
  var z = typeof y;
  print(y);
  x = y;
}

x = 1073741823; /* 2^30 - 1 */
y = pkg.add(x, 1);
print(x, " -> ", y);

x = -1073741823;
y = pkg.add(x, -1);
print(x, " -> ", y);
