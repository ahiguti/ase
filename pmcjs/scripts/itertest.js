
use('Perl');

var obj = {
  foo: 'val-foo',
  bar: 'val-bar',
  baz: 15
};

var arr = [
  "abc", "def", "ghi"
];


Perl.Use("IterTest");
var pkg = Perl.Functions["IterTest"];
var s = pkg.hjoin(obj);
print(s);
s = pkg.hjoin(arr);
print(s);
