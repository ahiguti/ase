#!/usr/bin/env perl 
use strict;
use warnings;

my %map = ();
gencpp('HTMLlat1.ent', \%map);
gencpp('HTMLspecial.ent', \%map);
gencpp('HTMLsymbol.ent', \%map);

foreach my $key (sort(keys %map)) {
  my $val = $map{$key};
  my $len = length($key);
  print "  { \"$key\", $len, $val },\n";
}

sub gencpp {
  my $mapr = $_[1];
  open(FH, $_[0]);
  while (my $line = <FH>) {
    chomp($line);
    if ($line =~ /^\<\!ENTITY\s+(\w+)\s+CDATA\s+\"\&\#(\d+)\;\"/) {
      $mapr->{$1} = $2;
    }
  }
  close(FH);
}

