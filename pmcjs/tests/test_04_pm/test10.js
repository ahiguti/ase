
var filename = "dbmfilter.out";

use('Perl');
Perl.Use('GDBM_File');
var wrcr = Perl.Constants['GDBM_File'].GDBM_WRCREAT;
var db = Perl.NewHash();
var tx = Perl.Tie(db, 'GDBM_File', filename, wrcr, 0640);

var filter_store = function(_) { print("STORE[" + _ + "]"); return 'X' + _; }
tx.filter_store_value(filter_store);

db['foo'] = 'bar';
for (var i in db) {
  print("db[" + i + "] = " + db[i]);
}

