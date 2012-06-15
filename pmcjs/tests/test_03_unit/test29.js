
use('Perl');
Perl.Use('FuncTest');
Perl.Functions['FuncTest'].foo('abc', 'def');
Perl.FunctionsLC['FuncTest'].foo('abc', 'def');
Perl.Methods['FuncTest'].foo('abc', 'def');
Perl.MethodsLC['FuncTest'].foo('abc', 'def');
Perl.Constants['FuncTest'].foo;
Perl.ConstantsLC['FuncTest'].foo;
var fooarr = Perl.Arrays['FuncTest'].fooarr;
for (var i = 0; i < fooarr.length; ++i) {
  print(fooarr[i]);
}
var foohash = Perl.Hashes['FuncTest'].foohash;
for (var i in foohash) {
  print(i + ": " + foohash[i]);
}
print(Perl.Scalars['FuncTest'].fooscalar);
Perl.Scalars['FuncTest'].fooscalar = 'def';
print(Perl.Scalars['FuncTest'].fooscalar);
