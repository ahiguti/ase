
use('Perl');
Perl.Use('File::Copy');
var fops = Perl.Functions['File::Copy'];
if (!fops.copy('abc.txt', 'cpy.out')) {
  print("FAILED");
} else {
  print("copied from abc.txt to cpy.out");
}

