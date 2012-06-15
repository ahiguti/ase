

use('Perl');
Perl.Use("DBI");
var dbh = Perl.Methods["DBI"].connect("dbi:SQLite:dbname=test.sq3","","");
dbh["do"]("DROP TABLE IF EXISTS t");
dbh["do"]("CREATE TABLE t(a text, b text)");
dbh.begin_work();
var sth = dbh.prepare("INSERT INTO t VALUES (?, ?)");
for (var i = 0; i < 1; ++i) {
  sth.bind_param(1, 10);
  sth.bind_param(2, i);
  sth.execute();
}
dbh.commit();
for (var j = 0; j < 10000; ++j) {
  sth = dbh.prepare("SELECT * FROM t");
  sth.execute();
  var row;
  while (row = sth.fetchrow_arrayref()) {
    for (var i = 0; i < row.length; ++i) {
//      print(row[i]);
    }
  }
}
// dbh["do"]("DROP TABLE t");

