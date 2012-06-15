
my $lib = $Native->{LibTest};
my $r;
for (my $i = 0; $i < 10000000; ++$i) {
  $r = $lib->abs(-10);
}
print "$r\n";

