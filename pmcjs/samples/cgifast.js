#!/usr/bin/ssjs

use('Perl');
Perl.Use('CGI::Fast');
var syswrite = Perl.Eval('sub { syswrite STDOUT, $_[0]; }');
var reqcount = 0;
while (true) {
  try {
    new Perl('CGI::Fast');
  } catch (e) {
    if (e.error == 'PerlNewFailed') {
      break;
    }
    throw e;
  }
  syswrite("Content-Type: text/html\r\n\r\n" + (++reqcount));
  if ((reqcount & 1023) == 0) {
    gc();
  }
}

