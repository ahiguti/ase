
package ATie;

sub TIEARRAY {
  my $classname = shift;
  print "tiearray\n";
  my @arr = ();
  my $obj = bless \@arr, $classname;
  bless $obj, $classname;
  return $obj;
}

sub FETCH {
  my $obj = shift;
  my $key = shift;
  print "fetch [$key]\n";
  return $$obj[$key];
}

sub STORE {
  my $obj = shift;
  my $key = shift;
  my $val = shift;
  print "store [$key] [$val]\n";
  $$obj[$key] = $val;
}

sub FETCHSIZE {
  my $obj = shift;
  print "fetchsize\n";
  return scalar(@$obj);
}

sub STORESIZE {
  my $obj = shift;
  my $len = shift;
  print "storesize [$len]\n";
}

1;

