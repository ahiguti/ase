
// var x = Perl.NewHash({ foo: 'fooval', bar: 'barval' });
var x = Perl.Eval("{ foo => 'foooo' }");
print("foo: [" + x.foo + "]");
print("x.Detach()");
try {
  x.Detach();
} catch (e) {
  print("err: " + e.error);
}
print("foo: [" + x.foo + "]");
print("Perl.Detach(x)");
Perl.Detach(x);
print("foo: [" + x.foo + "]");
