
use('Perl');
Perl.Use("Net::HTTP");
var s = new Perl("Net::HTTP", "Host", "www.perl.com");
s.write_request("GET", "/", "User-Agent", "Mozilla/5.0");
var r = s.InvokeLC("read_response_headers");
var code = r[0];
var mess = r[1];
var str = "";
while (true) {
  var buf = Perl.NewScalar();
  var n = s.read_entity_body(buf, 1024);
  if (!n) {
    break;
  }
  str += buf;
}
print(str);

