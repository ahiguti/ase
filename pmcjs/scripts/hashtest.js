
use('Perl');

var x = Perl.NewHash({ foo : 'fooval', bar : 'barval', あ : 'あval' });
print(x);
for (var i in x) {
  print("k=[" + i + "] v=[" + x[i] + "]");
}
x = Perl.NewHash([ 12, 94, 117, "abc", "999" ]);
print(x);
for (var i in x) {
  print("k=[" + i + "] v=[" + x[i] + "]");
}

