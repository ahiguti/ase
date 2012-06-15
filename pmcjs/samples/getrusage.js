
use('Perl');
Perl.Use('BSD::Resource');
var getrusage = Perl.FunctionsLC['BSD::Resource'].getrusage;

for (var i = 0; i < 100000; ++i) {
  var x = new Object();
}

var rusage = getrusage();
print('usertime=' + rusage[0]);
print('systemtime=' + rusage[1]);
print('minflt=' + rusage[6]);
print('majflt=' + rusage[7]);

