
use('Perl');
Perl.Use('Archive::Zip');
var zipcs = Perl.Constants['Archive::Zip'];
var zip = new Perl('Archive::Zip');
if (zip.read('zipsample.zip') != zipcs.AZ_OK) {
  throw "failed to read zipsample.zip";
}
var lst = zip.InvokeLC('memberNames');
print("contents of zipsample.zip:");
for (var i in lst) {
  print(lst[i]);
}

