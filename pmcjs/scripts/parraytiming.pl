
my $z;
for (my $lp = 0; $lp < 1000; ++$lp) {
  my @x = ();
  for (my $i = 0; $i < 1000; ++$i) {
    $x[$i] = $i;
  }
  $z = scalar @x;
}
print "$z\n";
