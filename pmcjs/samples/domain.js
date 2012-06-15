
use('Perl');
Perl.Use('Net::Domain');
var fs = Perl.Functions['Net::Domain'];
print(fs.hostfqdn());
print(fs.hostname());
print(fs.hostdomain());

