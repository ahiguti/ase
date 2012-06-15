
Perl.Use('CacheTest');
var writehash = Perl.Functions['CacheTest'].writehash;
var ph = Perl.NewHash();
// ph['foo'] = 'fooval';
// ph['bar'] = 'barval';
ph.foo = 'fooval';
ph.bar = 'barval';
print(ph['foo']);
print(ph['bar']);
writehash(ph);
print(ph['foo']);
print(ph['bar']);

