
use('Perl');

Perl.Use("FuncObj");
var f = function(x, y, z) { return x + y + z; }
var x = Perl.Functions["FuncObj"];
var z = 0;
for (var i = 0; i < 1000000; ++i) {
  z = x.testfunc(f);
}

print(z);
