
var hostname = arguments[0];
if (!hostname) {
  throw "Usage: socket.js HOSTNAME";
}

use("Perl");
Perl.Use("IO::Socket::INET");
var sock = new Perl("IO::Socket::INET",
  'PeerHost', hostname,
  'PeerPort', 80);
sock.send("GET / HTTP/1.0\r\n\r\n");
var buf = Perl.NewScalar();
sock.recv(buf, 1024);
print(buf);

