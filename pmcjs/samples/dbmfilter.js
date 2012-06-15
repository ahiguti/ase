
var filename = "dbmfilter.out";

use('Perl');
Perl.Use('DBM_Filter');
Perl.Use('GDBM_File');
var wrcr = Perl.Constants['GDBM_File'].GDBM_WRCREAT;
var db = Perl.NewHash();
var tx = Perl.Tie(db, 'GDBM_File', filename, wrcr, 0640);

var filter_fetch = function(_) { print("FETCH[" + _ + "]"); return _; }
var filter_store = function(_) { print("STORE[" + _ + "]"); return 'X' + _; }

/* filters will be applied twice */
tx.Filter_Value_Push('Fetch', filter_fetch, 'Store', filter_store);
tx.Filter_Value_Push('Fetch', filter_fetch, 'Store', filter_store);

db['foo'] = 'bar';
for (var i in db) {
  print("db[" + i + "] = " + db[i]);
}

print("wrote " + filename);

