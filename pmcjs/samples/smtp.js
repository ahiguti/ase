
var mailfrom = arguments[0];
var mailto = arguments[1];
var message = arguments[2];
if (!mailfrom || !mailto || !message) {
  throw "Usage: smtp.js FROM@DOMAIN TO@DOMAIN RAW_MESSAGE";
}

use('Perl');
Perl.Use('Net::SMTP');
var smtp = new Perl('Net::SMTP', 'localhost'); // use local smtpd
if (!smtp.mail(mailfrom)) {
  throw "MAIL failed";
}
if (!smtp.recipient(mailto)) {
  throw "RCPT failed";
}
if (!smtp.data()) {
  throw "DATA failed";
}
smtp.datasend(message);
if (!smtp.dataend()) {
  throw "dataend failed";
}
smtp.quit();

