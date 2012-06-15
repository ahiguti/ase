
package DelTest;

sub delprop {
  my $obj = shift;
  my $key = shift;
  delete $obj->{$key};
};

1;
