
use GDBM_File;
my %ht = ();
my $gdbm = tie %ht, 'GDBM_File', 'test_gdbm.out', &GDBM_WRCREAT, 0640;

# raw pointer is copied by value
use Storable;
my $cp = Storable::dclone($gdbm);

# crashes because of double-close

