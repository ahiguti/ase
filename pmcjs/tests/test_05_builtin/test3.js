
var core = Perl.Functions['CORE'];

function dumphash(x, mess) {
  if (mess) {
    print_nonl(mess + ": ");
  }
  print_nonl('{');
  for (var i in x) {
    print_nonl(' ' + i + ': ' + x[i]);
  }
  print(' }');
}

function dumparr(x, mess) {
  if (mess) {
    print_nonl(mess + ": ");
  }
  print_nonl('[');
  for (var i in x) {
    print_nonl(' ' + x[i]);
  }
  print(' ]');
}

var hash = Perl.NewHash({ foo: 'fooval', bar: 'barval' });
dumparr(core.keys(hash));
dumparr(core.values(hash));

var hash = Perl.NewHash({ foo: 'fooval', bar: 'barval' });
dumphash(hash);
dumphash(core.reverse(hash));

