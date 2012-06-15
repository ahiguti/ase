
sub foo { }
my $x = \&foo;
print "$x\n";
my $y = 0 + $x;
print "$y\n";
if ($x == $y) {
  print("equal\n");
}
my $sx = ref($x);
my $sy = ref($y);
print "refy=$sx\n";
print "refy=$sy\n";
