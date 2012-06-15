
var filename = arguments[0];
if (!filename) {
  throw "Usage: filetype.js FILENAME";
}

use('Perl');
Perl.Use("File::Type");
var ft = new Perl('File::Type');
var typ = ft.checktype_filename(filename);
print(filename + ": " + typ);

