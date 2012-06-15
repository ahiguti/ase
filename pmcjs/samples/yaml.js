
use('Perl');
Perl.Use('YAML');
var yaml = Perl.Functions['YAML'];
var obj = { apple: 'good', banana: 'bad', cauliflower: 'ugly' };
print(yaml.Dump(obj));
print(yaml.Dump(Perl.NewHash(obj)));

