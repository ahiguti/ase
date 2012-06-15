
my $filename = "dbfile.out";

use DB_File;
my %db = ();
my $x = tie %db, 'DB_File', $filename, O_CREAT, 0640, $DB_BTREE;

use Storable;
my $y = Storable::dclone($x); # causes double-close

