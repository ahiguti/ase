
use('Perl');
var env = Perl.Hashes[""].ENV;
for (var i in env) {
  print(i + " = " + env[i]);
}

