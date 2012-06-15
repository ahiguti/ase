
use('Perl');

Perl.Use('Locale::Country');
var country = Perl.Functions['Locale::Country'];
var c = country.code2country('jp');
print('jp -> ' + c);
print();
var codes = Perl.FunctionsLC['Locale::Country'].all_country_codes();
var names = Perl.FunctionsLC['Locale::Country'].all_country_names();

for (var i in codes) {
  print(codes[i]);
}
for (var i in names) {
  print(names[i]);
}
