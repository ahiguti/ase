
use('Perl');
// Perl.Eval("sub foo() { print(\"abc\\n\"); }");
Perl.Eval("sub foo() { }");
var fc = Perl.Functions[""];
var f = fc.foo;
for (var i = 0; i < 1000000; ++i) {
  f();
  //fc.foo();
}

