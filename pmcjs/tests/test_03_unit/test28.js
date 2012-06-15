
use('Perl');
var testfunc = Perl.Eval("sub { return 'あいうえお'; }");
print(testfunc());
var x = Perl.SuppressConv(function() { return testfunc(); });
dump(x.GetText());
print();
dump(x.GetBinary());
print();
