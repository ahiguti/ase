
var obj = {
  foo: 'val-foo',
  bar: 'val-bar',
  baz: 15
};

var arr = [
  "abc", "def", "ghi"
];


Perl.Use("IterTest");
var s = Perl.Functions["IterTest"].hjoin(obj);
print(s);
s = Perl.Functions["IterTest"].ajoin(arr);
print(s);
