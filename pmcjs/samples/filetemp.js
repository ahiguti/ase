
use('Perl');
Perl.Use('File::Temp');

var fh = new Perl('File::Temp',
  'TEMPLATE', 'tmpXXXXXX',
  'DIR', '.',
  'SUFFIX', '.out');
print(fh.filename());
fh.print("abcdef");
fh.unlink_on_destroy(0);
fh.close();

