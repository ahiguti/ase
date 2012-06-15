
use('Perl');
if (0) {
  var chomp = Perl.Eval('sub { return CORE::chomp($_[0]); }');
  var chop = Perl.Eval('sub { return CORE::chop($_[0]); }');
  var chr = Perl.Eval('sub { return CORE::chr($_[0]); }');
  var crypt = Perl.Eval('sub { return CORE::crypt($_[0], $_[1]); }');
  var hex = Perl.Eval('sub { return CORE::hex($_[0]); }');
  var index = Perl.Eval('sub { return CORE::index($_[0]); }');
  var lc = Perl.Eval('sub { return CORE::lc($_[0]); }');
  var lcfirst = Perl.Eval('sub { return CORE::lcfirst($_[0]); }');
  var length = Perl.Eval('sub { return CORE::length($_[0]); }');
  var oct = Perl.Eval('sub { return CORE::oct($_[0]); }');
  var ord = Perl.Eval('sub { return CORE::ord($_[0]); }');
  var reverse = Perl.Eval(
    'sub {' +
    ' my $x = $_[0];' +
    ' my $xr = ref($x);' +
    ' if ($xr eq \'HASH\' || $xr eq \'JavaScript::HWrap\') {' +
    '  my %r = CORE::reverse(%$x);' +
    '  return \\%r;' +
    ' } elsif ($xr eq \'ARRAY\' || $xr eq \'JavaScript::AWrap\') {' +
    '  my @r = CORE::reverse(@$x);' +
    '  return \\@r;' +
    ' }' +
    ' return CORE::reverse($_[0]); }'
    );
  var rindex = Perl.Eval(
    'sub {' +
    ' if (scalar(@_) < 3) { return CORE::rindex($_[0], $_[1]); }' +
    ' return CORE::rindex($_[0], $_[1], $_[2]);' +
    '}');
  var sprintf = Perl.Eval( 'sub { return CORE::sprintf(CORE::shift, @_); }');
  var substr = Perl.Eval(
    'sub {' +
    ' if (scalar(@_) >= 4) {' +
    '   return CORE::substr($_[0], $_[1], $_[2], $_[3]); }' +
    ' if (scalar(@_) == 3) { return CORE::substr($_[0], $_[1], $_[2]); }' +
    ' return CORE::substr($_[0], $_[1]);' +
    '}');
  var uc = Perl.Eval('sub { return CORE::uc($_[0]); }');
  var ucfirst = Perl.Eval('sub { return CORE::ucfirst($_[0]); }');
  var pop = Perl.Eval('sub { return CORE::pop(@{$_[0]}); }');
  var push = Perl.Eval('sub { return CORE::push(@{CORE::shift()}, @_); }');
  var shift = Perl.Eval('sub { return CORE::shift(@{$_[0]}); }');
  var splice = Perl.Eval(
    'sub {' +
    ' if (scalar(@_) >= 4) {' +
    '   return CORE::splice(@{CORE::shift()}, CORE:shift, CORE*:shift, @_); }' +
    ' if (scalar(@_) == 3) { return CORE::splice(@{$_[0]}, $_[1], $_[2]); }' +
    ' if (scalar(@_) == 2) { return CORE::splice(@{$_[0]}, $_[1]); }' +
    ' return CORE::splice(@{$_[0]}, $_[1]);' +
    '}');
  var unshift = Perl.Eval('sub { return CORE::unshift(@{CORE::shift()}, @_); }');
  var grep = Perl.Eval('sub { return CORE::grep(&{$_[0]}, @{$_[1]}); }');
  var join = Perl.Eval('sub { return CORE::join($_[0], @{$_[1]}); }');
  var map = Perl.Eval(
    'sub {' +
    ' my @r = CORE::map(&{$_[0]}, @{$_[1]});' + 
    ' return \\@r;' +
    '}');
  var sort = Perl.Eval(
    'sub {' +
    '  my ($x, $y) = @_;' +
    '  my $s = sub ($$) { return $x->($_[0], $_[1]); };' +
    '  my @z = sort $s @$y;' +
    '  return \\@z;' +
    '}');
  var pack = Perl.Eval('sub { return CORE::pack(CORE::shift, @_); }');
  var unpack = Perl.Eval(
    'sub {' +
    ' my @r = CORE::unpack($_[0], $_[1]);' +
    ' return \\@r;' +
    '}');
  var keys = Perl.Eval(
    'sub {' +
    ' my @r = CORE::keys(%{$_[0]});' +
    ' return \\@r' +
    '}');
  var values = Perl.Eval(
    'sub {' +
    ' my @r = CORE::values(%{$_[0]});' +
    ' return \\@r' +
    '}');

  // not necessary?
  var fork = Perl.Eval('sub { return CORE::fork(); }');
  var alarm = Perl.Eval('sub { return CORE::alarm($_[0]); }');
  var exec = Perl.Eval('sub { return CORE::exec(@{$_[0]}); }'); // FIXME
  var system = Perl.Eval('sub { return CORE::system(@{$_[0]}); }'); // FIXME
  var wait = Perl.Eval('sub { return CORE::wait(); }');
  var waitpid = Perl.Eval('sub { return CORE::waitpid($_[0], $_[1]); }');
}

function dumparr(x) {
  for (var i in x) {
    print(x[i]);
  }
}

// var arr = [ 99, 98, 97 ];
var arr = Perl.NewArray([ 99, 98, 97 ]);
var ht = { foo: 'fooval', bar: 'barval' };
// var ht = Perl.NewHash({ foo: 'fooval', bar: 'barval' });

var c = Perl.Functions['CORE'];
var p = Perl.Functions['POSIX'];

if (0) {
  var x = c.keys(ht);
  dumparr(x);
  x = c.values(ht);
  dumparr(x);
}

if (0) {
  c.system(['ls', '-la']);
}
if (0) {
  c.system('ls', '-la');
}

if (0) {
  var pid = c.fork();
  if (pid == 0) {
    print('this is child');
  } else {
    print('this is parent');
  }
}

if (0) {
  var s = c.pack('CCCC', 65, 66, 67, 68);
  print(s);
  dumparr(c.unpack('CCCC', s));
}

if (0) {
  var a2 = c.map(function(_) { return _ * 2; }, arr);
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

if (1) {
  var arr = Perl.NewArray([ 10, 300, 90, 15, 30 ]);
  function f() {
  //var f = function(_) {
    print("_ = " + Perl.ARG);
    return Perl.ARG > 30;
  }
  var x = c.grep(f, arr);
  // var x = c.grep(f, [50, 30, 20]);
  dumparr(x);
}

