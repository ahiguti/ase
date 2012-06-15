#!/usr/bin/perl

use Tie::Hash;
my %myenv = ();
tie %myenv, Tie::StdHash;

use FCGI;
my $stdin = \*STDIN;
my $stdout = \*STDOUT;
my $stderr = \*STDERR;
my $env = \%myenv;
my $request = FCGI::Request($stdin, $stdout, $stderr, $env);

my $count = 0;
while($request->Accept() >= 0) {
  print("Content-type: text/html\r\n\r\n", ++$count);
}

