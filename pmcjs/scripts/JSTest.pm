
package JSTest;

sub new {
  my $class = shift;
  my $jsobj = shift;
  my $self = {};
  $self->{'jsobj'} = $jsobj;
  $self->{'propx'} = "this is propx";
  bless($self);
  return $self;
}

sub testprop {
  my $self = shift;
  my $jsobj = $self->{'jsobj'};
  return $jsobj->{'foo'};
  #return $jsobj->GetProperty('foo');
}

sub testrun {
  my $self = shift;
  my $methodname = shift;
  my $jsobj = $self->{'jsobj'};
  print "jsobj=$jsobj\n";
  $jsobj->Invoke($methodname);
  return $self;
}

1;

