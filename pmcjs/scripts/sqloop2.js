
use('Perl');

Perl.Use("DBI");
var dbh = Perl.Methods["DBI"].connect("dbi:SQLite:dbname=test.sq3","","");
for (var i = 0; i < 10000; ++i) {
  dbh["do"]("SELECT 3");
}
dbh.disconnect();
dbh.Detach();
dbh = null;
