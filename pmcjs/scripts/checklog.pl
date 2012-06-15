#!/usr/bin/perl -w

my %allocmap;
my %privmap;
my $nalloc = 0;
my $nfree = 0;
my $nspriv = 0;
my $nfpriv = 0;
while (<>) {
  chomp;
  my @arr = split(/ /);
  my $cmd = $arr[0];
  my $addr = $arr[1];
  my $idstr = $arr[2];
  if ($cmd eq "__m") {
    my $ov = $allocmap{$addr};
    if ($ov) {
      print "addr $addr already alloced old=$ov cur=$idstr\n";
    }
    $allocmap{$addr} = "id:" . $idstr;
    $nalloc++;
  } elsif ($cmd eq "__f") {
    my $ov = $allocmap{$addr};
    if (!$ov) {
      print "addr $addr is not alloced cur=$idstr\n";
    }
    $allocmap{$addr} = 0;
    $nfree++;
  } elsif ($cmd eq "__spriv") {
    my $ov = $privmap{$addr};
    if ($ov) {
      print "priv $addr already alloced old=$ov cur=$idstr\n";
    }
    $privmap{$addr} = "id:" . $idstr;
    $nspriv++;
  } elsif ($cmd eq "__fpriv") {
    my $ov = $privmap{$addr};
    if (!$ov) {
      print "priv $addr is not alloced cur=$idstr\n";
    }
    $privmap{$addr} = 0;
    $nfpriv++;
  }
}
print "nalloc = $nalloc\n";
print "nfree = $nfree\n";
print "nspriv = $nspriv\n";
print "nfpriv = $nfpriv\n";

while (($key, $val) = each %allocmap) {
  if ($val) {
    print "addr $key leaks $val\n";
  }
}

while (($key, $val) = each %privmap) {
  if ($val) {
    print "priv $key leaks $val\n";
  }
}
