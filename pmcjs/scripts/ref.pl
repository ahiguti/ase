
my @arr = (10, 20, 30);
my %ht = (a => 10, b => 20);
# my $x = \@arr;
my $x = \%ht;
print ref($x), "\n";
if (defined(@$x)) {
  print "arrref\n";
}
