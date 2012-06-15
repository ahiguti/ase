
use('Perl');
Perl.Use('Extract');
var fs = Perl.Functions['Extract'];
var arr = [0, 2, 4, 6];
var obj = {a: 'b', c: 'd'};
var fnc = function(x, y) { return x + y; }
print(arr.toSource());
print(obj.toSource());
print(fnc.toSource());
var arrx = fs.f(arr);
var objx = fs.f(obj);
var fncx = fs.f(fnc);
print(arrx.toSource());
print(objx.toSource());
print(fncx.toSource());

if (arrx == arr) {
  print("arr extract ok");
} else {
  print("arr extract FAILED");
  print_address(arr);
  print_address(arrx);
}
if (objx == obj) {
  print("obj extract ok");
} else {
  print("obj extract FAILED");
  print_address(obj);
  print_address(objx);
}
if (fncx == fnc) {
  print("fnc extract ok");
} else {
  print("fnc extract FAILED");
  print_address(fnc);
  print_address(fncx);
}

