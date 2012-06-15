
package CacheTest;

sub writehash {
  my $h = shift;
  my $i;
  foreach $i (keys %$h) {
    $h->{$i} = "wrpl";
  }
}

1;
