
%ht = (
  'foo' => 31,
  'bar' => 12,
  'baz' => 99,
);

my $is_first = 1;
while (($k1, $v1) = each %ht) {
  if ($is_first) {
    delete $ht{$k1};
    $is_first = 0;
  }
  print "$k1 $v1\n";
}

