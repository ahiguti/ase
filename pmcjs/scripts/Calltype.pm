
package Calltype;

sub test_calltype {
  my $fns = shift;
  my $pkg = $fns->{'Calltype'};
  my $f = $pkg->{'foo'};
  my $d = $f->("abcdef");
  print "$d\n";
}

sub foo {
  return "foo retval";
}

1;
