
use DBI;
for (my $i = 0; $i < 10000; $i++) {
  my $dbh = DBI->connect("dbi:SQLite:dbname=test.sq3","","");
  $dbh->prepare("SELECT 3")->execute();
  $dbh->disconnect();
}

