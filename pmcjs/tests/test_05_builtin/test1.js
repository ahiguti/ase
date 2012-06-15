
var core = Perl.Functions['CORE'];

var pvar = Perl.NewScalar("abcdef\n");
dump(pvar); print();
core.chomp(pvar);
dump(pvar); print();
pvar = Perl.NewScalar("abcdef\n");
core.chop(pvar);
dump(pvar); print();

print(core.chr(65));
print(core.crypt('foobar', 'aa'));
print(core.hex('ffff'));
print(core.index('abcdefabcdef', 'abc', 1));
print(core.index('abcdefabcdef', 'abc'));
print(core.lc('AbCdEf'));
print(core.lcfirst('AbCdEf'));
print(core.length('abcdef'));
print(core.length('あいうえお')); /* should be 5 */
print(core.oct('777'));
print(core.ord('A').toString(16)); /* 0x41 */
print(core.ord('あ').toString(16)); /* 0x3042 */
print(core.reverse('abcdef'));
print(core.rindex('abcdefabcdef', 'abc', 1));
print(core.rindex('abcdefabcdef', 'abc'));
print(core.sprintf('x=%d y=%d', 10, 20));
print(core.substr('abcdef', 2, 2));
print(core.substr('abcdef', 2));

var pvar = Perl.NewScalar("abcdef");
print(core.substr(pvar, 2, 2, "ABCD"));
print(pvar);

