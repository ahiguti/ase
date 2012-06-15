
package TieTest::HTie;

my $verbose = 0;

sub TIEHASH {
  my $classname = shift;
  my $opt = shift;
  if (scalar(@_) != 0) { print "remaining args: @_\n" };
  my $this = { a => "b" };
  $verbose = $opt;
  return bless $this, $classname;
}

sub DELETE {
  my $this = shift;
  my $key = shift;
  print "tiehash delete [$key]\n" if $verbose;
  delete $this->{$key};
}

sub FIRSTKEY {
  my $this = shift;
  my $a = keys %$this;
  my $r = each %$this;
  print "tiehash firstkey = $r\n" if $verbose;
  return $r;
}

sub NEXTKEY {
  my $this = shift;
  my $r = each %$this;
  print "tiehash nextkey = $r\n" if $verbose;
  return $r;
}

sub FETCH {
  my $this = shift;
  my $key = shift;
  print "tiehash fetch [$key]\n" if $verbose;
  return $this->{$key};
}

sub STORE {
  my $this = shift;
  my $key = shift;
  my $val = shift;
  print "tiehash store [$key] [$val]\n" if $verbose;
  $this->{$key} = $val;
}

package TieTest::ATie;

my $verbose = 0;

sub TIEARRAY {
  my $classname = shift;
  my $opt = shift;
  my @arr = ();
  $verbose = $opt;
  return bless \@arr, $classname;
}

sub DELETE {
  my $this = shift;
  my $key = shift;
  print "tiearr delete [$key]\n" if $verbose;
  delete $this->[$key];
}

sub FETCH {
  my $this = shift;
  my $key = shift;
  print "tiearr fetch [$key]\n" if $verbose;
  return $this->[$key];
}

sub STORE {
  my $this = shift;
  my $key = shift;
  my $val = shift;
  print "tiearr store [$key] [$val]\n" if $verbose;
  $this->[$key] = $val;
}

sub FETCHSIZE {
  my $this = shift;
  #die "agrrrrr!";
  return scalar(@$this);
}

sub STORESIZE {
  my $this = shift;
}

package TieTest::STie;

my $verbose = 0;
my $die_fetch = 0;
my $die_store = 0;

sub TIESCALAR {
  my $classname = shift;
  $verbose = shift;
  $die_fetch = shift;
  $die_store = shift;
  my @arr = (123);
  return bless \@arr, $classname;
}

sub FETCH {
  my $this = shift;
  print "tiescalar fetch\n" if $verbose;
  die "agrrrr" if $die_fetch;
  return $this->[0];
}

sub STORE {
  my $this = shift;
  my $val = shift;
  die "agrrrr" if $die_store;
  print "tiescalar store [$val]\n" if $verbose;
  $this->[0] = $val * 2;
}

package TieTest;

sub tietest {
  my $opt = shift;
  my %h = ();
  tie %h, "TieTest::HTie", $opt;
  return \%h;
}

sub newht {
  shift;
  my $opt = shift;
  my %h = ();
  tie %h, "TieTest::HTie", $opt;
  return bless \%h;
}

sub newat {
  shift;
  my $opt = shift;
  my @a = ();
  tie @a, "TieTest::ATie", $opt;
  return bless \@a;
}

sub foo {
  shift;
  my $x = shift;
  print "foo invoked\n";
  return $x * 2;
}

1;

