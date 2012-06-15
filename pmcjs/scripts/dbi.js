
use("Perl");
Perl.Use("DBI");
var x = Perl.FunctionsLC["DBI"].available_drivers();
for (var i in x) {
  print(x[i]);
}

