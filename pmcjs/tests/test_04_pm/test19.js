
use('Perl');
Perl.Use('File::Compare');
var compare_text = Perl.Functions['File::Compare'].compare_text;

var cmpf0 = function(a, b) {
  print("a=" + a + "b=" + b);
  return a != b;
}
var cmpf1 = function(a, b) { return a != b; }
var cmpf2 = function(a, b) { return a.toLowerCase() != b.toLowerCase(); }

var c0 = compare_text("compare1.txt", "compare2.txt", cmpf0);
var c1 = compare_text("compare1.txt", "compare2.txt", cmpf1);
var c2 = compare_text("compare1.txt", "compare2.txt", cmpf2);
print("c1=" + c1 + " c2=" + c2);
