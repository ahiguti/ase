
use('Perl');
Perl.Use('File::Path');
var mkpath = Perl.Functions['File::Path'].mkpath;

mkpath(['tmp/foo/bar/baz', 'tmp/fubar/'], 1, 0700);

