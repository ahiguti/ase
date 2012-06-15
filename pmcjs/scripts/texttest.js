
use('Perl');

Perl.Use("TextTest");
var tt = Perl.Functions["TextTest"];
var r = tt.setbinmode();
tt.print("abc");
var r = tt.print("あいうえお");
print_nonl("ret = ");
dump(r);
print();

