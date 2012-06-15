
package MethodTest;

sub new {
  my $classname = shift;
  my %ht = ();
  $ht{'count'} = 0;
  return bless \%ht;
}

sub foo {
  my $obj = shift;
  my $c = $obj->{'count'}++;
  print "foo is called\n";
  return $c;
}

1;
