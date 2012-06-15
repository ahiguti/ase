
sub fib {
  my ($n) = @_;
  return 1 if $n < 2;
  return fib($n - 1) + fib($n - 2);
}

my $r = fib(32);
print "$r\n";
