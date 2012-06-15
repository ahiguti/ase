
use('Perl');
try {
  Perl.Eval("JavaScript->Call('foo');");
} catch (e) {
  print("exception: " + e);
}
