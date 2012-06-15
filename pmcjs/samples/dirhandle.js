
use('Perl');
Perl.Use("DirHandle");
var d = new Perl("DirHandle", "..");
while (n = d.read()) {
  print("ent: ", n);
}
