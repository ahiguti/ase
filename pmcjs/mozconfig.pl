#!/usr/bin/env perl

my $PREFIX = "/usr";
my $PKG = "firefox-runtime";
#my $PREFIX = "/usr/local";
#my $PKG = "firefox";

use File::Glob ':glob';
if (!defined $ARGV[0]) {
  die "specify 'inc', 'libs', or 'idl'";
}
my @ds;
if ($ARGV[0] eq "inc") {
  @ds = bsd_glob("$PREFIX/include/$PKG*");
} elsif ($ARGV[0] eq "libs") {
  @ds = bsd_glob("$PREFIX/lib*/$PKG*");
} elsif ($ARGV[0] eq "idl") {
  @ds = bsd_glob("$PREFIX/share/idl/$PKG*");
} else {
  die "specify 'inc', 'libs', or 'idl'";
}
my $len = scalar @ds;
die "mozilla not found" if ($len < 0);
print "$ds[$len - 1]";

