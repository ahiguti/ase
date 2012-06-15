
use('Perl');
Perl.Use("CGI::Pretty");
Perl.Scalars["CGI::Pretty"].INDENT = "-";
var p = Perl.Functions["CGI::Pretty"];
var x = p.table(p.TR(p.td("foo")));
print(x);

