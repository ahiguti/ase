
var d = new Date();
var dt = d.getTime;
for (var i = 0; i < 10; ++i) {
  var x = dt();
}
print(x);
