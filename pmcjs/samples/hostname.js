
use('Perl');
Perl.Use("Sys::Hostname");
var host = Perl.Functions["Sys::Hostname"].hostname();
print(host);

