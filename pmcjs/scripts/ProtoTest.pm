
package ProtoTest;

sub mypush(\@@) {
  my $r = shift;
  push @$r, @_;
}

1;
