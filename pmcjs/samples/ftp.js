
var host;
if (arguments[0]) {
  host = arguments[0];
} else {
  throw "Usage: ftp.js HOSTNAME";
}

use('Perl');

var stdin = Perl.Handles[''].STDIN;
var stdout = Perl.Handles[''].STDOUT;

Perl.Use('POSIX');
var psf = Perl.Functions['POSIX'];
var psc = Perl.Constants['POSIX'];

function set_echo(flag) {
  var termios = new Perl('POSIX::Termios');
  termios.getattr();
  var lf = termios.getlflag();
  if (flag) {
    lf |= psc.ECHO;
  } else {
    lf &= ~(psc.ECHO);
  }
  termios.setlflag(lf);
  termios.setattr();
}

Perl.Use('Net::FTP');
try {
  var ftp = new Perl('Net::FTP', host);
} catch (e) {
  throw "ftp: failed to connect '" + host + "'";
}

stdout.print("login: ");
var user = stdin.getline().replace('\n', '');
stdout.print("password: ");
try {
  set_echo(0);
  var passwd = stdin.getline().replace('\n', '');
  stdout.print("\n");
} finally {
  set_echo(1);
}
if (!ftp.login(user, passwd)) {
  throw "ftp: login failed";
}
var arr = ftp.dir();
for (var i in arr) {
  print(arr[i]);
}
ftp.close();

