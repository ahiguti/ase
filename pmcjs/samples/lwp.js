
var url = arguments[0];
if (!url) {
  throw "Usage: lwp.js URL";
}

use('Perl');
Perl.Use('LWP::UserAgent');
var ua = new Perl('LWP::UserAgent');
ua.agent = 'MyApp/0.1 ';
var req = new Perl('HTTP::Request', 'GET', url);
var resp = ua.request(req);
if (resp.is_success()) {
  print(resp.content());
} else {
  print(resp.status_line());
}

