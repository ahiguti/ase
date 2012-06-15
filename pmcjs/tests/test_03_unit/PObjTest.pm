
package PObjTest;

sub new {
  $instance = {
    x => 'this is prop-x',
    y => 'this is prop-y',
    z => 'this is prop-z',
  };
  return bless $instance;
}

sub foo {
  my ($instance, $x, $y, $z) = @_;
  return $x + $y + $z;
}

sub bar {
  my ($instance, $x, $y, $z) = @_;
  return $x . $y . $z;
}

sub tostr {
  my $instance = shift;
  my $i;
  my $str = "";
  foreach $i (keys %$instance) {
    $str .= "($i val=$instance->{$i}) ";
  }
  return $str;
}

1;

