
package MethodTiming;

sub new {
  return bless {};
}

sub foo {
  return 3;
}

sub callfrompl {
  my $num = shift;
  my $obj = MethodTiming->new();
  my $r = 0;
  for (my $i = 0; $i < $num; ++$i) {
    $r = $obj->foo();
  }
  return $r;
}

1;
