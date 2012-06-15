
use('Perl');

Perl.Use("DBI");
for (var i = 0; i < 10000; ++i) {
  var dbh = Perl.Methods["DBI"].connect("dbi:SQLite:dbname=test.sq3","","");
  dbh["do"]("SELECT 3");
  dbh.disconnect();
  dbh.Detach();
  dbh = null;
}

