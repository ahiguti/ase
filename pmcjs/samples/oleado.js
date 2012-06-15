
use('Perl');
Perl.Use('Win32::OLE');
Perl.Methods['Win32::OLE'].Option('CP', Perl.Constants['Win32::OLE'].CP_UTF8);
Perl.Use('Win32::OLE::Variant');
var Variant = Perl.Functions['Win32::OLE::Variant'];

var db = new Perl('Win32::OLE', 'ADODB.Connection');
db.Provider = 'Microsoft.Jet.OLEDB.4.0';
db.Mode = 1;
db.ConnectionString = 'adotest.mdb';
db.Open;

var rs = db.Execute('select * from testtbl');
var n = rs.Fields.Count;
for (var i = 0; i < n; ++i) {
  print('fld ' + i + ': ' + rs.Fields.Item(i).Name);
}
while (!rs.EOF) {
  print();
  var rec = rs.Fields;
  for (var i = 0; i < n; ++i) {
    var val = rec.Item(i).Value;
    if (Perl.IsPerl(val)) {
      var v = Variant.Variant(Variant.VT_DATE(), val);
      if (v) {
	val = v.Date('yyyy/MM/dd') + ' ' + v.Time('HH:mm:ss');
      } else {
        val = null;
      }
    }
    print('value: ' + val);
  }
  rs.MoveNext;
}
db.Close;
print('done');

