
use('Perl');
Perl.Use('Archive::Tar');
var lst = Perl.MethodsLC['Archive::Tar'].list_archive('tarsample.tar.gz', 1);
print("contents of tarsample.tar.gz:");
for (var i in lst) {
  print(lst[i]);
}

