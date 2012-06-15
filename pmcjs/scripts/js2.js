
use('Perl');

var obj = {
  foo : "3",
  bar : "4",
  methodfoo : function(x) { print("mf: " + x); },
};

Perl.Use("JSTest");
var x = new Perl("JSTest", obj);
x.testrun("methodfoo", "abc");
var y = x["propx"];
print(y);
var z = x.GetProperty("propx");
print(z);

var p = x.testprop();
print("jsobjprop ", p);
