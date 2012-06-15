
use ATie;
my @arr = ();
tie @arr, 'ATie';
$arr[10] = 3;
print "$arr[10]\n";

my $idx = 1;
for ($i = 0; $i < 100; ++$i) {
  print "[$idx] [$arr[$idx - 1]]\n";
  $idx = $idx + $idx;
}

