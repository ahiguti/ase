
use('Perl');
Perl.Use('GlobalTest');
function foo() {
  print("foo is called");
}
Perl.Functions["GlobalTest"].runtest();

