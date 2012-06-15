
function foo(x, y, z) {
  return x + y + z;
}

var r = Perl.Apply(foo, [10, 20, 30]);
print(r);

