
x = function(a, b) { return a + b; }
var y = Perl.CoerceToFunction(x);
print(x);
print(y);
var r = y(10, 30);
print("r=" + r);

