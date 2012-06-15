
use('Perl');
Perl.Use("File::Basename");
var pkg = Perl.Functions["File::Basename"];
var x = "/foo/bar/abcdef.ghi";
var y = pkg.basename(x, '.ghi');
var z = pkg.dirname(x);
print("x=[" + x + "] y=[" + y + "] z=[" + z + "]");

