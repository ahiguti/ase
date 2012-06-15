
use('Perl');
Perl.Use('File::Copy');
var fops = Perl.Functions['File::Copy'];
if (!fops.copy('copytest.txt', 'cpy.out')) {
  print("FAILED");
} else {
  print("copied from copytest.txt to cpy.out");
}

