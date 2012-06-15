#!/usr/bin/ssjs

use('Perl');
Perl.Use('IO::Handle');
Perl.Use('FCGI');
var env = Perl.NewHash();
var hin = new Perl('IO::Handle');
var hout = new Perl('IO::Handle');
var herr = new Perl('IO::Handle');
var fcgi = Perl.Functions['FCGI'];
var request = fcgi.Request(hin, hout, herr, env);
var count = 0;
while (request.Accept() >= 0) {
  var s = '';
  for (var i in env) {
    s += i + '=' + env[i] + '\r\n';
  }
  hout.print("Content-Type: text/plain\r\n\r\n" + s);
}

