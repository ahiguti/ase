
var country1 = arguments[0];
var country2 = arguments[1];
if (!country1 || !country2) {
  throw 'Usage: soaplite.js COUNTRY1 COUNTRY2\n' +
    '  e.g., soaplite.js USA Japan\n';
}

use('Perl');
Perl.Use('SOAP::Lite');
var SOAPLite = Perl.Methods['SOAP::Lite'];
var SOAPData = Perl.Methods['SOAP::Data'];
var r = SOAPLite
  .uri('urn:xmethods-CurrencyExchange')
  .proxy('http://services.xmethods.net/soap')
  .getRate(
    SOAPData.name('country1', country1),
    SOAPData.name('country2', country2))
  .result();
print('Currency rate for ' + country1 + '/' + country2 + ' is  ' + r);

