
package JSCallback;

sub new {
  my $class = shift;
  my $jsobj = shift;
  my $self = {};
  $self->{'jsobj'} = $jsobj;
  bless($self);
  return $self;
}

sub invokejs {
  my $self = shift;
  my $methodname = shift;
  my $jsobj = $self->{'jsobj'};
  print "jsobj=$jsobj\n";
  return $jsobj->Invoke($methodname, @_);
}

sub foo {
  my $self = shift;
  my $jsobj = $self->{'jsobj'};
  return $jsobj->foo(@_);
}

1;
