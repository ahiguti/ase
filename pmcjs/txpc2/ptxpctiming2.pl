
my $jsgl = $JavaScript::Global;
my $x = $jsgl->{'Components'}->{'classes'}->{'txpc'}->createInstance()
  ->{'ITXPC'};
for (my $i = 0; $i < 1000000; ++$i) {
  $x->setValue(100);
}
print("getValue: ", $x->getValue(), "\n");
