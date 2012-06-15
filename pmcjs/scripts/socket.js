
use("Perl");
Perl.Use("IO::Socket::INET");
var sock = new Perl("IO::Socket::INET", "www.yahoo.com:80");
sock.send("GET / HTTP/1.0\r\n\r\n");
var buf = Perl.NewScalar();
sock.recv(buf, 1024);
print(buf);

