
my $r = 0;
for (my $i = 0; $i < 10000; ++$i) {
  $r = 0;
  for (my $j = 1; $j <= 1000; ++$j) {
    $r += $j;
  }
}
print "$r\n";

