
use('Perl');
var my_sort = Perl.Eval(
  "sub { " +
  "  my ($x, $y) = @_;" +
  "  my $s = sub ($$) { return $x->($_[0], $_[1]); };" +
  "  my @z = sort $s @$y;" +
  "  return \\@z;" +
  "}");
var compare_func = function (x, y) {
  print("compare [" + x + "] [" + y + "]");
  if (x > y) return -1;
  if (x < y) return 1;
  return 0;
};
var arr = [ 'foo', 'bar', 'baz', 'fubar', '0', '99', 'ABCDE' ];
var x = my_sort(compare_func, arr);
for (var i in x) {
  print(i + ": " + x[i]);
}

