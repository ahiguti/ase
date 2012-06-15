
Perl.Use("XSTest");
var xt = new Perl("XSTest");
var f = Perl.Functions["XSTest"].setValue;
for (var i = 0; i < 1000000; ++i) {
  xt.Invoke("setValue", 1000);
}
print(xt.getValue());

