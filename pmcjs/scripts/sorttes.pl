
my $srt = sub {
  my $p = $_[0];
  my @r = sort @$p;
  return \@r;
};

sub mysrt {
  my $p = $_[0];
  my @r = sort @$p;
  return \@r;
};

my @arr = ( "abc", "def", "0", "XYZ", "ABC", "xyz" );
my $arrp = \@arr;
# my @x = sort @$arrp;
# my $r = \@x;
# my $r = mysrt($arrp);
my $r = $srt->($arrp);
for (my $i = 0; $i < @$r; ++$i) {
  print "$$r[$i]\n";
}
