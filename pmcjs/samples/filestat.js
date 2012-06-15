
use('Perl');
Perl.Use('File::stat');
var stat = Perl.Functions['File::stat'].stat;
var filename = 'filestat.js';
var st = stat(filename);
print("name=" + filename + " mode=" + st.mode().toString(8) +
  " nlink=" + st.nlink());

