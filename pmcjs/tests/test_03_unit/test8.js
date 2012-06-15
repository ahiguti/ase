
Perl.Use("NewTest");
var nt = Perl.Functions["NewTest"];
nt.newtest();

function Foobar(x) {
  this.propx = "this-is-propx";
  this.methody = function(a, b) {
    return a * b + x;
  }
}
nt.newtest_foobar();

var Foo = {
  Bar : function(x) {
    this.propy = "this-is-propy";
    this.methodz = function(a, b) {
      return a * b * x;
    }
  }
};
nt.newtest_fb();

