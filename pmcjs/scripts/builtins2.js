
use('Perl');
var core = Perl.Functions['CORE'];

function dumparr(x) {
  for (var i in x) {
    print(x[i]);
  }
}

// var arr = [ 99, 98, 97 ];
var arr = Perl.NewArray([ 99, 98, 97 ]);
var ht = { foo: 'fooval', bar: 'barval' };
// var ht = Perl.NewHash({ foo: 'fooval', bar: 'barval' });

if (1) {
  var x = core.keys(ht);
  dumparr(x);
  x = core.values(ht);
  dumparr(x);
}

if (0) {
  system(['ls', '-la']);
}

if (0) {
  var pid = fork();
  if (pid == 0) {
    print('this is child');
  } else {
    print('this is parent');
  }
}

if (0) {
  var s = pack('CCCC', 65, 66, 67, 68);
  print(s);
  dumparr(unpack('CCCC', s));
}

if (0) {
  var a2 = map(function(_) { return _ * 2; }, arr);
  dumparr(a2);
}

if (0) {
  print(join(':', arr));
}

if (0) {
  var x = reverse(arr);
  dumparr(x);
}

if (0) {
  var str = "abcdefabcdef";
  print(rindex(str, 'a'));
}

if (0) {
  print(sprintf("x=%d y=%d", 10, 20));
  var pstr = Perl.NewScalar("abcdef");
  substr(pstr, 2, 2, "xx");
  print(pstr);
  print(substr("abcdef", 2, 2));
  print(substr("abcdef", 2));
}

if (0) {
  push(arr, 150);
  splice(arr, 1, 2, 999, 998, 7);
  dumparr(arr);
  print(pop(arr));
}

if (0) {
  unshift(arr, 5, 3);
  dumparr(arr);
}

if (0) {
  var arr = Perl.NewArray([ 10, 300, 90, 15, 30 ]);
  function f(_) {
  //var f = function(_) {
    print("_ = " + _);
    return _ > 30;
  }
  print("grep");
  var x = grep(f, [50, 30, 20]);
  dumparr(x);
}

