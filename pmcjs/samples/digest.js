
use('Perl');
Perl.Use('Digest');
var funcs = ['MD5', 'SHA1', 'HMAC_MD5', 'HMAC_SHA1'];
var str = "foobar";
for (var i in funcs) {
  var f = funcs[i];
  var ctx = new Perl('Digest', f);
  ctx.add(str);
  var r = ctx.hexdigest();
  print(r);
}
