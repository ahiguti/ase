
my $UUID = $Native->{Util}->{UUID};
for (my $i = 0; $i < 10; ++$i) {
  my $x = $UUID->NewInstance();
  print $x->upper(), "\n";
}

