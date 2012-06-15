
use DBI;
my $dbh = DBI->connect("dbi:SQLite:dbname=test.sq3","","");
for (my $i = 0; $i < 10000; $i++) {
  $dbh->prepare("SELECT 3")->execute();
}
$dbh->disconnect();

