
use('Perl');
Perl.Use('IO::File');
var fh = new Perl("IO::File", "foo.txt", "a");
if (fh) {
  fh.print("foobar\n");
  fh.print("fubar\n");
  fh.close();
}

