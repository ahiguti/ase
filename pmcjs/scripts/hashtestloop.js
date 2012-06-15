
use('Perl');

var x;
for (var i = 0; i < 1000000; ++i) {
  x = Perl.NewHash({ foo : 'fooval', bar : 'barval', あ : 'あval' });
}
for (var i in x) {
  print("k=[" + i + "] v=[" + x[i] + "]");
}

