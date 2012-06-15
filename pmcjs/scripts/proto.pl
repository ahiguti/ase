
sub mypush(\@@) {
  my $ar = shift;
  push @$ar, @_;
}

my @arr = (10, 230, 99, 24);
mypush(@arr, 120, 300);
print "@arr\n";

