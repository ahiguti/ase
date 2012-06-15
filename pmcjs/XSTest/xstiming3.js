
Perl.Use("XSTest");
var xt = new Perl("XSTest");
var setValue = Perl.Functions["XSTest"].setValue;
for (var i = 0; i < 1000000; ++i) {
  // xt.setValue(1000);
  setValue(xt, 1000);
}
print(xt.getValue());

