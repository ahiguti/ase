

my $jsgl = $JavaScript::Global;
my $x = $jsgl->{Components}->{classes}->{txpc}->createInstance();

$x->setValue(100);
print("getValue: ", $x->getValue(), "\n");

sub Foo::new {
  return bless {}, 'Foo';
}
sub Foo::callback {
  print("Perl Foo::callback called\n");
}

my $cb = new Foo;
$x->doCallback($cb, 3);

sub Foo2::new {
  my $x = 0;
  return bless \$x, 'Foo2';
}
sub Foo2::callback {
  ++${$_[0]};
}

my $cb2 = new Foo2;
$x->doCallback($cb2, 9999);
print("cb2count: $$cb2\n");

my $cppcb = $x->createCallback(1);
$cppcb->callback();

1;

