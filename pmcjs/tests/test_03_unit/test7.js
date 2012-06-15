
var h = Perl.NewHash();
h["x"] = 3;
h["y"] = 5;
h["z"] = 10;
print("y=[" + h.y + "]");
delete h["y"];
print("y=[" + h.y + "]");

var a = Perl.NewArray();
a[0] = 10;
a[1] = 20;
a[2] = 30;
a[3] = 40;
print("a1=[" + a[1] + "]");
delete a[1];
print("a1=[" + a[1] + "]");

var obj = {
  foo: 'val-foo',
  bar: 'val-bar',
  baz: 'val-baz'
};

var arr = [
  "abc", "def", "ghi"
];


Perl.Use('DelTest');
print("baz=[" + obj.baz + "]");
Perl.Functions["DelTest"].delprop(obj, 'baz');
print("baz=[" + obj.baz + "]");

