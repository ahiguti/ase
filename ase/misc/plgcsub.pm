
package plgcsub;

sub new()
{
  return bless { value => $_[1] }, $_[0];
}

sub foo()
{
  my $value = $_[0]->{'value'};
  $value->();
}

1;
