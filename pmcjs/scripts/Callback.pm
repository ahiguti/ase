
package Callback;

sub callback() {
  my $obj = shift;
  for (my $i = 0; $i < 1000000; ++$i) {
    $obj->foo();
  }
}

1;

