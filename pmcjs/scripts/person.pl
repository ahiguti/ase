
use Person;
my $x = Person::new;
my $z;
for (my $i = 0; $i < 100000; $i++) {
  $x->name("XYZ");
  $z = $x->name();
}
print "$z\n";

