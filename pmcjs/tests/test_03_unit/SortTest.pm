
package SortTest;

sub new {
  my $classname = shift;
  my $cfunc = shift;
  if ($cfunc) {
    my $wrcf = sub {
      return $cfunc->($a, $b);
    };
    return sub {
      my $p = $_[0];
      my @r = sort $wrcf @$p;
      return \@r;
    };
  }
  return sub {
    my $p = $_[0];
    my @r = sort @$p;
    return \@r;
  };
};

1;

