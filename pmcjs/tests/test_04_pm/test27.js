
use('Perl');
Perl.Use('Net::servent');
var servent = Perl.Functions['Net::servent'];
var s = servent.getservbyname('ftp');
print("name=" + s.name());
print("port=" + s.port());

