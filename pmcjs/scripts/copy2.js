
use('Perl');
Perl.Use('File::Copy');
var fops = Perl.Functions['File::Copy'];
var stdout = Perl.Handles[''].STDOUT;
if (!fops.copy('copytest.txt', stdout)) {
  print("FAILED");
} else {
  print("copied from copytest.txt to stdout");
}

