
my $lib = $Native->{LibTest};
my $r;
for (my $i = 0; $i < 10000000; ++$i) {
  $r = $lib->foo(3, 5);
}
print "r=$r\n";
