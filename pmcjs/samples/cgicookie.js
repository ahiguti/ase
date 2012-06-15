
use('Perl');
Perl.Use('CGI');
var cgi = Perl.Functions['CGI'];
Perl.Use('CGI::Cookie');
var cookie = new Perl('CGI::Cookie',
  '-name', 'foo',
  '-value', 'bar',
  '-expires', '+3M',
  '-domain', '.example.com',
  '-path', '/cgi-bin/foobar',
  '-secure', 1);
var s = "";
s += cgi.header('-cookie', cookie);
print(s);

