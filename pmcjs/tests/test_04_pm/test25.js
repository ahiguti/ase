
use('Perl');

Perl.Use('POSIX');
var setlocale = Perl.Functions['POSIX'].setlocale;
var cs = Perl.Constants['POSIX'];
setlocale(cs.LC_ALL, 'ja_JP.UTF-8');

Perl.Use('I18N::Langinfo');
var li = Perl.Constants['I18N::Langinfo'];
var langinfo = Perl.Functions['I18N::Langinfo'].langinfo;

print("yesstr=" + Perl.BinToText(langinfo(li.YESSTR)));

