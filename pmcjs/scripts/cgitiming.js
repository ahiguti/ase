
use('Perl');

Perl.Use("CGI::Pretty");
var p = Perl.Functions["CGI::Pretty"];
var v = Perl.Scalars["CGI::Pretty"];
for (var i = 0; i < 1000000; ++i) {
  try {
    p.INDENT = " ";
  } catch (e) {
  }
}
var x = p.table(p.TR(p.td("foo")));
print(x);
print("[" + v.INDENT + "]");

