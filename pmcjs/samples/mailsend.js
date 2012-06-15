
var mailto = arguments[0];
var message = arguments[1];
if (!mailto || !message) {
  throw "Usage: mailsend.js MAILTO MESSAGE";
}

use('Perl');
Perl.Use('Mail::Send');
var msg = new Perl('Mail::Send');
msg.subject('testmail from mailsend.js');
msg.to(mailto);
var fh = msg.open();
fh.print(message);
fh.close();

