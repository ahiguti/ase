
use('Perl');
Perl.Use("MIME::Base64");
var b64 = Perl.Functions["MIME::Base64"];
var s1 = "abcdef";
var s2 = b64.encode_base64(s1);
var s3 = b64.decode_base64(s2);
print(s1);
print(s2);
print(s3);

