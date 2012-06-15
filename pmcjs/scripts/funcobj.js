
use('Perl');

Perl.Use("FuncObj");
var f = function(x, y, z) { return x + y + z; }
//var wrappedf = Perl.Methods["JavaScript"].FuncToSub(f);
// Perl.Functions["FuncObj"].testfunc(wrappedf);
var x = Perl.Functions["FuncObj"];
print(x.testfunc(f));

