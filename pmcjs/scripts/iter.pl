
%ht = (
  'foo' => 31,
  'bar' => 12,
  'baz' => 99,
);

foreach my $k1 (keys %ht) {
  foreach my $k2 (keys %ht) {
    print "$k1 $ht{$k1} $k2 $ht{$k2}\n";
  }
}

while (($k1, $v1) = each %ht) {
  #while (($k2, $v2) = each %ht) {
  #  print "$k1 $v1 $k2 $v2\n";
  #}
}
