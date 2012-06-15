
use('Perl');

var obj = {
  foo : function(x, y) { return x + y; },
  bar : function() { return "bar is called"; }
};

Perl.Use("JSCallback");
var pc = new Perl("JSCallback", obj);
var x = pc.invokejs("foo", 100, 300);
print(x);
var x = pc.invokejs("foo", "abc", "def");
print(x);
var x = pc.foo(33, 22);
print(x);
