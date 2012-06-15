use DBI;
my $dbh = DBI->connect('dbi:DBM:');
$dbh->{RaiseError} = 1;
for my $sql( split /;\n+/,"
  CREATE TABLE user ( user_name TEXT, phone TEXT );
  INSERT INTO user VALUES ('Fred Bloggs','233-7777');
  INSERT INTO user VALUES ('Sanjay Patel','777-3333');
  INSERT INTO user VALUES ('Junk','xxx-xxxx');
  DELETE FROM user WHERE user_name = 'Junk';
  UPDATE user SET phone = '999-4444' WHERE user_name = 'Sanjay Patel';
  SELECT * FROM user
  "){
  my $sth = $dbh->prepare($sql);
  $sth->execute;
  $sth->dump_results if $sth->{NUM_OF_FIELDS};
}
$dbh->disconnect;

