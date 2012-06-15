
use('Perl');
Perl.Use('IO::Zlib');

var fh = new Perl('IO::Zlib');
fh.open('zlibtest.txt.gz', 'r');
var s;
while (s = fh.getline()) {
  print(s);
}
fh.close();

