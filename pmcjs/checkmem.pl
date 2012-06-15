#!/usr/bin/perl -w

my %allocmap;
my %privmap;
my $nalloc = 0;
my $nfree = 0;
my $nspriv = 0;
my $nfpriv = 0;
my $errflag = 0;
while (<>) {
  chomp;
  my ($cmd, $addr, $idstr) = split(/ /);
  if (!defined($addr)) {
    next;
  }
  if (!defined($idstr)) {
    $idstr = "";
  }
  if ($cmd eq "__m") {
    my $ov = $allocmap{$addr};
    if ($ov) {
      print "addr $addr already alloced old=$ov cur=$idstr\n";
      $errflag = 1;
    }
    $allocmap{$addr} = "id:" . $idstr;
    $nalloc++;
  } elsif ($cmd eq "__f") {
    my $ov = $allocmap{$addr};
    if (!$ov) {
      print "addr $addr is not alloced cur=$idstr\n";
      $errflag = 1;
    }
    $allocmap{$addr} = 0;
    $nfree++;
  } elsif ($cmd eq "__spriv") {
    my $ov = $privmap{$addr};
    if ($ov) {
      print "priv $addr already alloced old=$ov cur=$idstr\n";
      $errflag = 1;
    }
    $privmap{$addr} = "id:" . $idstr;
    $nspriv++;
  } elsif ($cmd eq "__fpriv") {
    my $ov = $privmap{$addr};
    if (!$ov) {
      print "priv $addr is not alloced cur=$idstr\n";
      $errflag = 1;
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

my $r = ($errflag == 0 && $nalloc == $nfree && $nspriv == $nfpriv);

exit($r ? 0 : 1);

