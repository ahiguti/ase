
var filename = 'sampleutf.txt';
var hfname = 'MD5';

use('Perl');
Perl.Use('Digest::file');
var s = Perl.Functions['Digest::file'].digest_file_hex(filename, hfname);
print(s);

