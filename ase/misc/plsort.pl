
my $Comparable = $GCJ->{'java.lang.Comparable'};
my $Arrays = $GCJ->{'java.util.Arrays'};

sub Foo::new {
  return bless { value => $_[1] }, $_[0];
}
sub Foo::get {
  return $_[0]->{value};
}
sub Foo::compareTo {
  my ($x, $y) = @_;
  print "compareTo: $x->{value} $y->{value}\n";
  if ($x->{value} < $y->{value}) { return -1; }
  if ($x->{value} > $y->{value}) { return 1; }
  return 0;
}

my $len = 20;
my $arr = $Comparable->NewArray($len);
for (my $i = 0; $i < $len; ++$i) {
  $arr->{$i} = new Foo(int(rand(1000)));
}
for (my $i = 0; $i < $len; ++$i) {
  my $v = $arr->{$i}->get();
  print "i=$i v=$v\n";
}

print "sort\n";
$Arrays->sort($arr);
for (my $i = 0; $i < $len; ++$i) {
  my $v = $arr->{$i}->get();
  print "i=$i v=$v\n";
}

