
use('Perl');
Perl.Use('Term::Cap');
var t = Perl.Methods['Term::Cap'].Tgetent();
t.Trequire('cl', 'cm');
var s = "";
s += t.Tputs('cl', 1);
s += t.Tgoto('cm', 20, 5);
print(s + '<- here is (20, 5)');

