
use('Perl');
Perl.Use('Env');
Perl.Methods['Env']['import']('@PATH');

var ptarr = Perl.Arrays[''].PATH;
for (var i in ptarr) {
  print(i + " = " + ptarr[i]);
}

