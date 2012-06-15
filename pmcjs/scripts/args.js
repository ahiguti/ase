
use('Perl');
print("args:");
for (var i in arguments) {
  print(arguments[i]);
}
var pargv = Perl.Arrays[''].ARGV;
print("perl args:");
for (var i in pargv) {
  print(pargv[i]);
}
