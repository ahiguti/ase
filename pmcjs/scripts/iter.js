
use('Perl');

var m = {
  foo : 31,
  bar : 12,
  baz : 99
};

var i;
for (i in m) {
  for (j in m) {
    print("" + i + " " + m[i] + " " + j + " " + m[j]);
  }
}

