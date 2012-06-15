
use GDBM_File;
my %ht = ();
my $gdbm = tie %ht, 'GDBM_File', 'test_gdbm.out', &GDBM_WRCREAT, 0640;
$gdbm->close();
# crashes because of double-close

