
use('Perl');
Perl.Use('File::DosGlob');
var dosglob = Perl.FunctionsLC['File::DosGlob'].glob;
var arr = dosglob('*.js *.txt');
for (var i in arr) {
  print(arr[i]);
}

