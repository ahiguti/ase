
use('Perl');
Perl.Use('File::Spec');

var s = Perl.Methods['File::Spec'].catfile('a', 'b', 'c');
print(s);

