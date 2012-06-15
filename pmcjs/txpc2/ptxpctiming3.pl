
my $jsgl = $JavaScript::Global;
my $x = $jsgl->{'Components'}->{'classes'}->{'txpc'}->createInstance()
  ->{'ITXPC'};
my $cbcount = 0;
my $cb = {
  callback => sub { ++$cbcount; }
};
$x->doCallback($cb, 1000000);
print("$cbcount\n");
