
var x = Perl.Eval("20 * 30");
print(x);

Perl.Use("EvalTest");
try {
  Perl.Functions["EvalTest"].evaltest();
} catch (e) {
  print("evaltest: exception: ", e);
}
