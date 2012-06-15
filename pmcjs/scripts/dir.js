
use('Perl');
Perl.Use("IO::Dir");
var d = new Perl("IO::Dir", "..");
while (n = d.read()) {
  print("ent: ", n);
}
