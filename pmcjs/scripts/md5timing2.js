
use('Perl');

Perl.Use("Digest::MD5");

var str = "abcdef";
var d2;
var pkg = Perl.FunctionsRaw["Digest::MD5"];
for (var i = 0; i < 1000000; ++i) {
  d2 = pkg.md5_base64(str);
//  var ctx = new Perl("Digest::MD5");
//  ctx.add(str);
//  var d2 = ctx.b64digest();
}
print("r=", d2);

