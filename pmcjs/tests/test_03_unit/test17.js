
Perl.Use("FuncObj");
var f = function(x, y, z) { return x + y + z; }
var x = Perl.Functions["FuncObj"];
print(x.testfunc(f));

