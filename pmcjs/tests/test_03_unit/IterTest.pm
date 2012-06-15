
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

sub ajoin {
  my $obj = shift;
  my $key;
  my $s = "";
  for (my $i = 0; $i < scalar(@$obj); ++$i) {
    my $val = $obj->[$i];
    $s .= " ($i $val)";
  }
  return $s;
};
1;
