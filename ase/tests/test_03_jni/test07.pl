
my $Comparable = $ASE->{Java}->{"java.lang.Comparable"};
my $Arrays = $ASE->{Java}->{"java.util.Arrays"};

sub Foo::new {
  return bless { value => $_[1] }, $_[0];
}
sub Foo::compareTo {
  my ($x, $y) = @_;
  print "compareTo: $x->{value} $y->{value}\n";
  if ($x->{value} < $y->{value}) { return -1; }
  if ($x->{value} > $y->{value}) { return 1; }
  return 0;
}

my $rndval = 999;
my $len = 20;
my $arr = $Comparable->NewArray($len);
for (my $i = 0; $i < $len; ++$i) {
  $arr->{$i} = $Comparable->NewInstance(new Foo($rndval));
  $rndval = ($rndval * 69069 + 1) % 2147483648;
}
for (my $i = 0; $i < $len; ++$i) {
  my $v = $arr->{$i}->{value};
  print "i=$i v=$v\n";
}

$Arrays->sort($arr);

for (my $i = 0; $i < $len; ++$i) {
  my $v = $arr->{$i}->{value};
  print "i=$i v=$v\n";
}

