
use('Perl');

var h = Perl.NewHash();
h["abc"] = 12;
h["xyz"] = 103;
print(h["abc"]);
print(h["a3"]);
for (var i in h) {
  print("key=" + i + " val=" + h[i]);
}

