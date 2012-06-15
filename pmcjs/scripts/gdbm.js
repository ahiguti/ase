
var filename = "gdbmtest.out"

use('Perl');
Perl.Use('GDBM_File');
var wrcr = Perl.Constants['GDBM_File'].GDBM_WRCREAT;
var db = Perl.NewTiedHash('GDBM_File', filename, wrcr, 0640);

db["foo"] = "bar";
print("wrote gdbmtest.out");

