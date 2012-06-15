
my $jsgl = $JavaScript::Global;
my $x = $jsgl->{'Components'}->{'classes'}->{'txpc'}->createInstance();
my $cb = $x->createCallback(0);
$x->doCallback($cb, 1000000);
print("done\n");
