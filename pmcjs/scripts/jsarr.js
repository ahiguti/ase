
use('Perl');
var arr = [ 10, 20, 30 ];
var core = Perl.Functions['CORE'];
core.push(arr, 99);
core.push(arr, 3);
core.shift(arr);

print(arr);
var x = core.sort(['abc', '00', 'ABC', '99']);
for (var i in x) {
  print(x[i]);
}

