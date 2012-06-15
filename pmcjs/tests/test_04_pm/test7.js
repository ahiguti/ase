
var filename = "gdbmtest.out";

use('Perl');
Perl.Use('GDBM_File');
var wrcr = Perl.Constants['GDBM_File'].GDBM_WRCREAT;
// var db = Perl.NewHash();
// var tx = Perl.Tie(db, 'GDBM_File', filename, wrcr, 0640);
var db = Perl.NewTiedHash('GDBM_File', filename, wrcr, 0640);

for (var i = 0; i < 100; ++i) {
  db['foo'] = 'bar';
}
Perl.Detach(db);
// Perl.Detach(tx);

Perl.Use('GDBM_File');
var db2 = Perl.NewHash();
var tx2 = Perl.Tie(db2, 'GDBM_File', filename, wrcr, 0640);

var r;
for (var i in db2) {
  r = db2['foo'];
}
print("r=[" + r + "]");
