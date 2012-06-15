
sub fib {
  my $x = $_[0];
  if ($x <= 1) {
    return 1;
  } else {
    return fib($x - 1) + fib($x - 2);
  }
}

my $r = fib(30);
print "$r\n";

