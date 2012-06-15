
/* hashref */

use('Perl');
Perl.Use("DBI");
var dbh = Perl.Methods["DBI"].connect("dbi:SQLite:dbname=test.sq3","","");
dbh["do"]("DROP TABLE IF EXISTS t");
dbh["do"]("CREATE TABLE t(a,b)");
dbh.begin_work();
var sth = dbh.prepare("INSERT INTO t VALUES (?, ?)");
for (var i = 0; i < 100; ++i) {
  sth.bind_param(1, i);
  sth.bind_param(2, i * 2);
  sth.execute();
}
dbh.commit();
sth = dbh.prepare("SELECT * FROM t");
sth.execute();
print("sth=", sth);
var row;
while (row = sth.fetchrow_hashref()) {
  print(row["a"] + " " + row["b"]);
}
dbh.prepare("DROP TABLE t").execute();


