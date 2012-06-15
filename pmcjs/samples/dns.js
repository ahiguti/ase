
var domname = arguments[0];
if (!domname) {
  throw "Usage: dns.js DOMAINNAME";
}


use('Perl');
Perl.Use('Net::DNS');
var res = new Perl('Net::DNS::Resolver');
var resmx = Perl.FunctionsLC['Net::DNS'].mx;
var mxarr = resmx(res, domname);
if (!mxarr) {
  throw domname + ": no mx record found";
}
print(mxarr.length + " records found");
for (var i in mxarr) {
  var rec = mxarr[i];
  print(rec.preference + " " + rec.exchange);
}

