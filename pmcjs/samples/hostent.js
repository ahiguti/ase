
var hostname = arguments[0];
if (!hostname) {
  throw "Usage: hostent.js HOSTNAME";
}

use('Perl');

Perl.Use('Socket');
var sockfn = Perl.Functions['Socket'];

Perl.Use('Net::hostent');
var hostent = Perl.Functions['Net::hostent'];
var h = hostent.gethost(hostname);
var aliases = h.aliases();
var addr_list = h.addr_list();
print("name " + h.name());
for (var i in aliases) {
  print("alias " + aliases[i]);
}
for (var i in addr_list) {
  print("address " + sockfn.inet_ntoa(addr_list[i]));
}

