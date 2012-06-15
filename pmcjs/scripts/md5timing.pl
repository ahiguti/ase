
use Digest::MD5;

my $str = "abcdef";
my $d2;
for (my $i = 0; $i < 1000000; $i++) {
  $d2 = Digest::MD5::md5_base64($str);
#  my $ctx = Digest::MD5->new();
#  $ctx->add(str);
#  $d2 = $ctx->b64digest();
}
print($d2, "\n");

