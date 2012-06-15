
use('Perl');

Perl.Use("Person");
var x = new Perl("Person");
var z;
//for (var i = 0; i < 1000000; ++i) {
for (var i = 0; i < 10; ++i) {
  x.Invoke("name", "ABC");
  //z = x.Invoke("name");
  y = x.Invoke("x");
  z = y.Invoke("name");
  //x.name("XYZ");
  //z = x.name();
}
//var w = x.name(1, 2, 3, 4);
z;
