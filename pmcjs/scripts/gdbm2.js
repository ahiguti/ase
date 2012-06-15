
if (arguments.length < 1) {
  throw "Usage: gdbm.js FILENAME.dbm [ KEY [ VALUE ] ]";
}
var filename = arguments[0];
var key = arguments[1];
var val = arguments[2];

use('Perl');
Perl.Use('GDBM_File');
var wrcr = Perl.Constants['GDBM_File'].GDBM_WRCREAT;
var db = Perl.NewTiedHash('GDBM_File', filename, wrcr, 0640);

if (!key) {
} else if (!val) {
  delete db[key];
} else {
  db[key] = val;
}
for (var i in db) {
  print("db[" + i + "] = " + db[i]);
}

