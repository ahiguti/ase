use GD;
for (my $i = 0; $i < 10000; $i++) {
  $im = new GD::Image(100,100);
  $white = $im->colorAllocate(255,255,255);
  $black = $im->colorAllocate(0,0,0);
  $red = $im->colorAllocate(255,0,0);
  $blue = $im->colorAllocate(0,0,255);
  $im->transparent($white);
  $im->interlaced('true');
  $im->rectangle(0,0,99,99,$black);
  $im->arc(50,50,95,75,0,360,$blue);
  $im->fill(50,50,$red);
  $s = $im->png;
}
print "done\n";
#binmode STDOUT;
#print $im->png;

