
local obj = {
  foo = 'val-foo',
  bar = 'val-bar',
  baz = 15
};

Perl.Use("IterTest");
local pkg = Perl.Functions["IterTest"];
local s = pkg.hjoin(obj);
print(s);

