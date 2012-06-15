
use('Perl');
var callback = Perl.Eval(
  "sub { " +
  "  my $x = shift;" +
  "  $_ = 'underline';" + 
  "  $x->('arg1');" +
  "}");
function do_callback(n, f) {
  print(n);
  callback(f);
}

function foo_a(a) {
  print(a);
}
function foo_ul(_) {
  print(_);
}
function テスト_a(a) {
  print(a);
}
function テスト_ul(_) {
  print(_);
}

do_callback("anon_a", function(a) { print(a); });
do_callback("anon_ul", function(_) { print(_); });
do_callback("foo_a", foo_a);
do_callback("foo_ul", foo_ul);
do_callback("テスト_a", テスト_a);
do_callback("テスト_ul", テスト_ul);

