
my $jsgl = $JavaScript::Global;
my $x = $jsgl->{'Components'}->{'classes'}->{'txpc'}->createInstance();

$x->setValue(100);
print("getValue: ", $x->getValue(), "\n");

my $cb = {
  callback => sub { print("Perl function called\n"); }
};
$x->doCallback($cb, 3);

my $cb2count = 0;
my $cb2 = {
  callback => sub { ++$cb2count; }
};
$x->doCallback($cb2, 9999);
print("cb2count: $cb2count\n");

my $cppcb = $x->createCallback(1);
$cppcb->callback();

1;

