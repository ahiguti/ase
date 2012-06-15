
package MethodTiming2;

sub new {
  return bless {};
}

sub AUTOLOAD {
  my $al = our $AUTOLOAD;
  return if $al =~ /::DESTROY$/;
  return 3;
}

1;
