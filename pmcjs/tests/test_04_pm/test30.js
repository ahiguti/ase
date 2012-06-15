
use('Perl');

/* cloning a js object is forbidden */

Perl.Use('Storable');
var dclone = Perl.Functions['Storable'].dclone;

var arr = [ 'foo', 'bar', 100 ];
var arrx = dclone(arr);
arr[2] = 99;
print(arr[2]);
print(arrx[2]); /* returns undefined */

