
var core = Perl.Functions['CORE'];

function dumparr(x, mess) {
  if (mess) {
    print_nonl(mess + ": ");
  }
  print_nonl('[');
  for (var i = 0; i < x.length; ++i) {
    print_nonl(' ' + x[i]);
  }
  print(' ]');
}

function dotest_arr(f) {
  var arr = [ 99, 98, 97 ];
  // var arr = Perl.NewArray([ 99, 98, 97 ]);
  f(arr);
  dumparr(arr);
}

dotest_arr(function (arr) { print("pop=" + core.pop(arr)); });
dotest_arr(function (arr) { core.push(arr, 15); });
dotest_arr(function (arr) { print("shift=" + core.shift(arr)); });
dotest_arr(function (arr) { core.unshift(arr, 15); });

dumparr(core.reverse([ 10, 20, 30 ]));
print(core.join(':', [ 10, 20, 30 ]));
dumparr(core.map(function(x) { return x * 2; }, [ 10, 20, 30 ]));

var f1 = function() { return Perl.ARG > 30; }
var x = core.grep(f1, [ 10, 300, 90, 30, 31 ]);
dumparr(x);
var f2 = function(x) { return x > 30; }
var x = core.grep(f2, [ 10, 300, 90, 30, 31 ]);
dumparr(x);

var compare_func = function (x, y) {
  if (x > y) return -1;
  if (x < y) return 1;
  return 0;
};
var arr = [ 'foo', 'bar', 'baz', 'fubar', '0', '99', 'ABCDE' ];
var x = core.sort(compare_func, arr);
dumparr(x);

var s = core.pack('CCCC', [ 65, 66, 67, 68 ]);
print(s);
dumparr(core.unpack('CCCC', s));

function dosplice(f) {
  var arr = [ 99, 98, 97 ];
  // var arr = Perl.NewArray([ 99, 98, 97 ]);
  dumparr(arr, "sporig");
  var x = f(arr);
  dumparr(arr, "spsrc");
  dumparr(x, "spret");
}

dosplice(function(arr) { return core.splice(arr); });
dosplice(function(arr) { return core.splice(arr, 1); });
dosplice(function(arr) { return core.splice(arr, 1, 1); });
dosplice(function(arr) { return core.splice(arr, 1, 1, 999, 998, 997); });

