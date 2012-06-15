
%ht = (
  'foo' => 31,
  'bar' => 12,
  'baz' => 99,
);

my $is_first = 1;
foreach my $k1 (keys %ht) {
  if ($is_first) {
    delete $ht{'baz'};
    $is_first = 0;
  }
  foreach my $k2 (keys %ht) {
    print "$k1 $ht{$k1} $k2 $ht{$k2}\n";
  }
}

