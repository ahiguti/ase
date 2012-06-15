
use('Perl');

Perl.Use("NewTest");
var nt = Perl.Functions["NewTest"];
nt.newtest();

var Foo = {
  Bar : function(x) {
    this.propy = "this-is-propy";
    this.methodz = function(a, b) {
      return a * b * x;
    }
  }
};
var r = 0;
for (var i = 0; i < 1000000; ++i) {
  r = nt.newtest_fb();
}
print(r);
