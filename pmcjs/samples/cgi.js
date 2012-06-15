
use('Perl');
Perl.Use("CGI");
var q = new Perl("CGI");
var s = "";
s += q.header("-type", "text/html");
s += q.start_html('hello');
s += q.h1('hello');
s += q.end_html();
print(s);

