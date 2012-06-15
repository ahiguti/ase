
use XBase;
my $tbl = new XBase('addresses.dbf');
my $n = $tbl->last_record;
for (my $i = 0; $i <= $n; ++$i) {
  my $rec = $tbl->get_record_as_hash($i);
  print("record $i\n");
  my $k;
  foreach $k (keys %$rec) {
    my $v = $rec->{$k};
    print(" $k: $v\n");
  }
}
$tbl->close;

