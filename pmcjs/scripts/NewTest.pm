
package NewTest;

sub newtest {
  $r = JavaScript->new("Array", 10, 20, 30, 40, 50);
  $len = $r->{'length'};
  print "len=$len\n";
  foreach $i (keys %$r) {
    print "arr $r->{$i}\n";
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
#  $r = JavaScript->Call($f, 25);
  $r = JavaScript->new($f, 25);
  $z = $r->methodz(100, 5);
  return $z;
};

1;
