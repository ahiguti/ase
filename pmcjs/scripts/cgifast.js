#!/usr/bin/ssjs

use('Perl');
Perl.Use('CGI::Fast');
var syswrite = Perl.Eval('sub { syswrite STDOUT, @_[0]; }');
var count = 0;
while (new Perl('CGI::Fast')) {
  syswrite("Content-Type: text/html\r\n\r\n" + (++count));
}

