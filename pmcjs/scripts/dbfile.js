
var filename = "dbfile.out";

use('Perl');
Perl.Use('DB_File');
var cr = Perl.Constants['DB_File'].O_CREAT;
var db_btree = Perl.Scalars['DB_File'].DB_BTREE;
var db = Perl.NewHash();
var x = Perl.Tie(db, 'DB_File', filename, cr, 0640, db_btree);
db['foo'] = 'bar';
db['baz'] = 'fubar';
//x.put('foo', 'bar');

// Perl.Use('Storable');
// var dclone = Perl.Functions['Storable'].dclone;
// var y = dclone(x);
