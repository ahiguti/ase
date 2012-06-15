#!/usr/bin/env perl

use strict;

my %symtable = ();
my %traces = ();
my %sizes = ();

if (!defined($ARGV[0])) {
  die;
}

open(NM, "-|", "nm -C $ARGV[0]") or die "nm";
while (my $line = <NM>) {
  chomp($line);
  if ($line =~ /0*(\w+)\s\w\s(.*)/) {
    $symtable{$1} = $2;
  }
}
close(NM);

sub dump_trace {
  my ($str, $ptr, $siz, $trace) = @_;
  my @arr = reverse(split(' ', $trace));
  if (scalar(@arr) eq 0) {
    print "<$str pointer=\"$ptr\" size=\"$siz\"/>\n";
  } else {
    print "<$str pointer=\"$ptr\" size=\"$siz\">\n";
    foreach my $i (@arr) {
      if ($i =~ /[0x]*(.*)/ && defined($symtable{$1})) {
	print "  <symbol>$symtable{$1}</symbol>\n";
      } else {
	print "  <address>$i</address>\n";
      }
    }
    print "</$str>\n";
  }
}

open (LG, "/tmp/dbgml.out") or die "dbgml.out";
while (my $line = <LG>) {
  chomp($line);
  if ($line =~ /(\w)\s(\w+)\s(\w+)\s:\s(.*)/) {
    my $mf = $1;
    my $ptr = $2;
    my $siz = $3;
    my $trace = $4;
    if ($mf eq 'M') {
      if (defined($traces{$ptr})) {
        print "<warning>allocated-twice : $ptr</warning>\n";
      }
      $traces{$ptr} = $trace;
      $sizes{$ptr} = $siz;
    } else {
      if (!defined($traces{$ptr})) {
	dump_trace("invalid-free", $ptr, 0, $trace);
      }
      delete $traces{$ptr};
      delete $sizes{$ptr};
    }
  }
}
close(LG);

foreach my $key (keys %traces) {
  dump_trace("memory-leak", $key, $sizes{$key}, $traces{$key});
}

