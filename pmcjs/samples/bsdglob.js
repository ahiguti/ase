
use('Perl');
Perl.Use('File::Glob');
var bsd_glob = Perl.Functions['File::Glob'].bsd_glob;
var bsd_glob_lc = Perl.FunctionsLC['File::Glob'].bsd_glob;
var glob = Perl.Constants['File::Glob'];

var arr = bsd_glob_lc('../*.[ch]');
for (var i in arr) {
  print(arr[i]);
}
print();

var x = bsd_glob('~', glob.GLOB_TILDE);
print("your homedir=" + x);
