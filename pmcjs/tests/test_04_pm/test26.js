
use('Perl');
Perl.Use("MIME::QuotedPrint");
var qp = Perl.Functions["MIME::QuotedPrint"];
var s1 = "abc=def";
var s2 = qp.encode_qp(s1);
var s3 = qp.decode_qp(s2);
print(s1);
print(s2);
print(s3);

