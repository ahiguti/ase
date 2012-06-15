
var obj = {
  foo: 'val-foo',
  bar: 'val-bar',
  baz: 15
};

Perl.Use("IterTest");
var pkg = Perl.Functions["IterTest"];
var s = pkg.hjoin(obj);
print(s);

