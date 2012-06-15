use FCGI;

my $count = 0;
my $request = FCGI::Request();

while($request->Accept() >= 0) {
  print("Content-type: text/html\r\n\r\n", ++$count);
}

