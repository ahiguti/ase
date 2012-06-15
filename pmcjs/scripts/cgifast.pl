#!/usr/bin/env perl

use CGI::Fast;
my $count = 0;
while (new CGI::Fast) {
  print "Content-Type: text/html\r\n\r\n", ++$count;
}

