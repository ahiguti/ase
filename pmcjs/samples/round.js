
use('Perl');
Perl.Use('Math::Round');
var rnd = Perl.Functions['Math::Round'];
var vars = [ 1.0, 1.5, -1.5, 2.5, -2.5 ];
for (var i in vars) {
  var x = vars[i];
  print(x + " " + rnd.round(x) + " " + rnd.round_even(x));
}
