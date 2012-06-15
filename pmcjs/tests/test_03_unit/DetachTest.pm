
package DetachTest;

sub new {
  $klass = shift;
  $obj = shift;
  $instance = {
    foo => 'this is foo',
    jsobj => $obj,
  };
  return bless $instance;
};

sub getjsfoo {
  $instance = shift;
  my $jsobj = $instance->{'jsobj'};
  return $jsobj->getjsfoo();
}

sub getfoo {
  $instance = shift;
  return $instance->{'foo'};
}

1;
