
var username = 'daemon';

use('Perl');
Perl.Use('User::pwent');
var pw = Perl.Functions['User::pwent'].getpwnam(username);
print("uid = " + pw.uid());
print("name = " + pw.name());
print("dir = " + pw.dir());
print("shell = " + pw.shell());

