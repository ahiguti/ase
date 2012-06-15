
use('Perl');
Perl.Use("DBI");
var dbh = Perl.Methods["DBI"].connect("dbi:SQLite:dbname=test.sq3","","");
dbh["do"]("DROP TABLE IF EXISTS t");
dbh["do"]("CREATE TABLE t(a text, b text)");
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
var row;
while (row = sth.fetchrow_arrayref()) {
  var s = ""
  for (var i = 0; i < row.length; ++i) {
    s += row[i] + " ";
  }
  print(s);
}
dbh["do"]("DROP TABLE t");

