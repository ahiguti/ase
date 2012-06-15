
use Win32::OLE;
use Win32::OLE::Variant;
my $db = Win32::OLE->new('ADODB.Connection');
$db->{Provider} = 'Microsoft.Jet.OLEDB.4.0';
$db->{Mode} = 1;
$db->{ConnectionString} = 'adotest.mdb';
$db->Open;

my $rs = $db->Execute('select * from testtbl');
my $n = $rs->{Fields}->{Count};
for (my $i = 0; $i < $n; ++$i) {
  print('fld $i: ', $rs->{Fields}->Item($i)->{Name}, "\n");
}
while (!$rs->{EOF}) {
  print("\n");
  my $rec = $rs->{Fields};
  for (my $i = 0; $i < $n; ++$i) {
    my $v = $rec->Item($i)->{Value};
    if (ref($v)) {
      my $vd = Variant(VT_DATE, $v);
      $v = $vd->Date('yyyy/MM/dd') . ' ' . $vd->Time('HH:mm:ss');
    }
    print('value: ', $v, "\n");
  }
  $rs->MoveNext;
}
