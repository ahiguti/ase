
package CycleTest;

sub new {
  my $klass = shift;
  my $jso = shift;
  $instance = {
    jso => $jso,
    klass => $klass,
    mess => 'hey',
  };
  return bless $instance;
}

1;
