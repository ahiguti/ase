
use('Perl');

var m = {
  foo : 31,
  bar : 12,
  baz : 99
};

var is_first = true;
for (var i in m) {
  if (is_first) {
    delete m['baz'];
    is_first = false;
  }
  for (var j in m) {
    print("" + i + " " + m[i] + " " + j + " " + m[j]);
  }
}

