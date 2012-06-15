
use('Perl');
Perl.Use('Shell');
var shell = new Perl('Shell');
var s = shell.ls();
print("result:");
print(s);
