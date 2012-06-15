
use('Perl');
Perl.Use('Cwd');
var cwd = Perl.Functions['Cwd'];
print(cwd.getcwd());
print(cwd.abs_path('..'));
