
Perl.Use("XSTest");
var xt = new Perl("XSTest");
for (var i = 0; i < 1000000; ++i) {
  xt.setValue(1000);
}
print(xt.getValue());

