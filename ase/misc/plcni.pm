
package plcni;

sub run() {
  my $jsgl = $JavaScript::Global;
  my $testv = $jsgl->{'GCJ'}->{'asejava.TestVariant'}->NewInstance();
  print "testv=$testv\n";
  # my $r = $textv->(10.5);
  # print "r=$r\n";
  # return $r;
}

1;
