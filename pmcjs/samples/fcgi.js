#!/usr/bin/ssjs

use('Perl');
Perl.Use('FCGI');
var fcgi = Perl.Functions['FCGI'];
var request = fcgi.Request();
var syswrite = Perl.Eval('sub { syswrite STDOUT, $_[0]; }');
var count = 0;
while (request.Accept() >= 0) {
  syswrite("Content-Type: text/html\r\n\r\n" + (++count));
  if ((count & 1023) == 0) {
    gc();
  }
}

