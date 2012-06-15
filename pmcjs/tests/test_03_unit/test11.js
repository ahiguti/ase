
var jsobj = {
  jsfoo : 'this is jsfoo',
  getjsfoo : function() { return this.jsfoo; }
};

Perl.Use("DetachTest");
var x = new Perl("DetachTest", jsobj);
print("getfoo: " + x.getfoo());
print("getjsfoo: " + x.getjsfoo());

Perl.DetachJSProxies();
print("detachjs");
print("getfoo: " + x.getfoo());
try {
  print("getjsfoo: " + x.getjsfoo());
} catch (e) {
  print("getjsfoo exception: " + e);
}

Perl.DetachPerlProxies();
print("detachperl");
try {
  print("getfoo: " + x.getfoo());
} catch (e) {
  print("getfoo exception: " + e);
}

