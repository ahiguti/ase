#!/usr/bin/env perl

my %map;
my $line;

while ($line = readline(STDIN)) {
  chomp($line);
  if ($line =~ /^(\S+)\s+(\S+)/) {
    my $r = $map{$2};
    if (!$r) {
      $r = [];
      $map{$2} = $r;
    }
    push @$r, $1;
  }
}

my $i;
foreach $i (keys %map) {
  print "|| $i ||";
#  print "|| [http://ajaja.alphageek.jp/trac/browser/trunk/ajaja/pmcjs/samples/$i $i] ||";
  my $r = $map{$i};
  my $j;
  for ($j = 0; $j < scalar(@$r); ++$j) {
    my $v = $r->[$j];
    if ($j != 0) {
      print ",";
    }
#    print " $v";
    print " [/trac/browser/trunk/ajaja/pmcjs/samples/$v $v] ";
  }
  print " ||\n";
}

