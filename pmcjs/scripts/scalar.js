
use('Perl');

var x = Perl.NewScalar();
x.Set("abcdef");
print("" + x);
print(x.Get());
print(x);

