
use('Perl');

Perl.Use("FileHandle");
var fh = new Perl("FileHandle", "sampleutf.txt", "r");
var str_utf;
while (true) {
  var buf = Perl.NewScalar();
  var n = fh.read(buf, 1024);
  if (!n) {
    break;
  }
  str_utf += buf;
}
fh.close();

