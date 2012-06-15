
use('Perl');
Perl.Use('File::Find');
var find = Perl.Functions["File::Find"].find;
var vars = Perl.Scalars["File::Find"];
var wanted = function (_) {
  print("NAME: " + _);
  print("FULL: " + vars.name);
  print("DIR: " + vars.dir);
}
find(wanted, ".");
