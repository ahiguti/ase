
use('Perl');
Perl.Use('URI');
var uri = new Perl('URI', 'HTTP://WWW.perl.com:80');
print(uri.canonical().as_string());

