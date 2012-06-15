
use('Perl');

function dumparr(x, mess) {
  if (mess) {
    print_nonl(mess + ": ");
  }
  print_nonl('[');
  for (var i in x) {
    print_nonl(' ' + i + ' ' + x[i]);
  }
  print(' ]');
}


var core = Perl.Functions['CORE'];
var arr = [99, 98, 97];
var x = core.splice(arr, 1, 1, [999, 998, 997]);
dumparr(arr);
