
use('Perl');

Perl.Use('Locale::Country');
var country = Perl.Functions['Locale::Country'];
var c = country.code2country('jp');
print('jp -> ' + c);

