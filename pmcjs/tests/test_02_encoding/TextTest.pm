
package TextTest;
use strict;

sub setbinmode {
  binmode STDOUT => ":utf8";
}

sub print {
  my $s = $_[0];
  print utf8::is_utf8($s) ? "isutf: " : "not: ";
  print "$s\n";
  return $s;
}

1;  # so the require or use succeeds

