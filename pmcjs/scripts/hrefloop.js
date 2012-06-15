
use('Perl');

for (var i = 0; i < 1000000; ++i) {
  var x = Perl.NewHash();
  x.SetProperty("15", 3);
  x["15"] = 4;
  var y = x["15"];
}
print(y);

