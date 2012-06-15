
use('Perl');

Perl.Use("CGI::Pretty");
var p = Perl.Functions["CGI::Pretty"];
var v = Perl.Scalars["CGI::Pretty"];
for (var i = 0; i < 10000000; ++i) {
  v.INDENT = " ";
}
var x = p.table(p.TR(p.td("foo")));
print(x);
print("[" + v.INDENT + "]");

