
use('Perl');
Perl.Use('Convert::ASN1');
var cnv = new Perl('Convert::ASN1');
cnv.prepare(
  '[APPLICATION 7] ' +
  'SEQUENCE { ' +
  '  int INTEGER,' +
  '  strseq SEQUENCE OF OCTET STRING ' +
  '}');
var asn1bin = cnv.encode(
  'int', 7, 'strseq', [ 'foobar', 'fubar', 'あいうえお' ]);

Perl.Use('Data::HexDump');
var dmp = new Perl('Data::HexDump');
dmp.data(asn1bin);
print(dmp.dump());

