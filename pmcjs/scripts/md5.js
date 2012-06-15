
use('Perl');
Perl.Use("Digest::MD5");

var str = "abcdef";
var pkg = Perl.Functions["Digest::MD5"];
var digest = pkg.md5_hex(str);
print(digest);

var ctx = new Perl("Digest::MD5");
ctx.add(str);
var d2 = ctx.b64digest();
print(d2);

