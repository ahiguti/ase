
var mailto = arguments[0];
var message = arguments[1];
if (!mailto || !message) {
  throw "Usage: mailer.js MAILTO MESSAGE";
}

use('Perl');
Perl.Use('Mail::Mailer');
var mailer = new Perl('Mail::Mailer');
var headers = {
  To: mailto,
  Subject: 'testmail from mailer.js'
};
mailer.open(headers);
mailer.print(message);
mailer.close();

