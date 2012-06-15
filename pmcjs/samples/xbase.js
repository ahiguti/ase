
use('Perl');
Perl.Use('XBase');
try {
  var tbl = new Perl('XBase', 'addresses.dbf');
} catch (e) {
  throw Perl.Methods['XBase'].errstr();
}
var n = tbl.last_record();
for (var i = 0; i <= n; ++i) {
  var rec = tbl.get_record_as_hash(i);
  print('record ' + i);
  for (var j in rec) {
    print(' ' + j + ': ' + rec[j]);
  }
}
tbl.close();

