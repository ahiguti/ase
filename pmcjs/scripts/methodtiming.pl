
use MethodTiming;

my $x = MethodTiming->new();

my $r = 0;
for (my $i = 0; $i < 1000000; ++$i) {
  eval {
  $r = $x->foo();
  };
}

print($r, "\n");

