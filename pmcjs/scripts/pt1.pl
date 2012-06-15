
sub bar {
  print "arg0 = $_[1]\n";
};

sub foo {
  my $c = shift;
  my $obj = shift;
  return sub {
    if ($_[0] == \&foo && ref($_[0]) eq 'CODE') {
      print "CODEREF\n";
      return $obj;
    }
    return bar($obj, @_);
  };
}

my $x = foo("a", "b");
$x->("abc");

