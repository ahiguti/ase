
package FuncTest;

sub foo {
  my $argc = scalar(@_);
  my $arr = wantarray();
  print "foo: argc=$argc\n";
  print "wantarray=$arr\n";
  for (my $i = 0; $i < $argc; ++$i) {
    my $x = $_[$i];
    print "$i: $x\n";
  }
}

$fooscalar = 'abc';
@fooarr = ( 10, 20, 30 );
%foohash = ( 'a' => 10, 'b' => 20 );

1;


