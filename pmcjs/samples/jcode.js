
use('Perl');
Perl.Use('Jcode');
var str = 'あいうえおｱｲｳｴｵ漢字';
var jc = new Perl('Jcode', str);
jc.h2z();
var eucstr = jc.euc();

Perl.Use('Data::HexDump');
var f = new Perl('Data::HexDump');
f.data(eucstr);
print(f.dump());

