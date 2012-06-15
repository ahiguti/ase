
use('Perl');
Perl.Use('Sys::Syslog');
var syslog = Perl.Functions['Sys::Syslog'];
syslog.openlog('syslog.js', 'ndelay,pid', 'local0');
syslog.syslog('info', 'test message');
syslog.closelog();
print("wrote to syslog");
