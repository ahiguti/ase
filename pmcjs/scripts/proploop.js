
use('Perl');
var obj = Perl.NewHash();
var x;
for (var i = 0; i < 100000; ++i) {
  obj.foo = 3;
  x = obj.foo;
  delete obj.foo;
  obj.SetProperty('foo', 15);
  x = obj.GetProperty('foo');
  obj.DelProperty('foo');
}

