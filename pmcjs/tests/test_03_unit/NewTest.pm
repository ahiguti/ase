
package NewTest;

sub newtest {
  $r = JavaScript->new("Array", 10, 20, 30, 40, 50);
  $len = scalar(@$r);
  # $len = $r->GetProperty('length');
  print "len=$len\n";
  for (my $i = 0; $i < scalar(@$r); ++$i) {
    print "arr $r->[$i]\n";
  }
#  for ($i = 0; $i < $r->{'length'}; ++$i) {
#    print "arr $r->{$i}\n";
#  }
};

sub newtest_foobar {
  $r = JavaScript->new("Foobar", 3);
  $x = $r->{'propx'};
  $y = $r->methody(100, 200);
  print "x=$x y=$y\n";
};

sub newtest_fb {
  $f = JavaScript->Eval("Foo.Bar");
  $r = JavaScript->new($f, 25);
  $z = $r->methodz(100, 5);
  print "z=$z\n";
};

1;
