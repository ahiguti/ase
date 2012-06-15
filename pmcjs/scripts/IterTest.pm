
package IterTest;

sub hjoin {
  my $obj = shift;
  my $key;
  my $s = "";
  foreach $key (keys %$obj) {
    my $val = $obj->{$key};
    $s .= " ($key $val)";
  }
  return $s;
};

1;
