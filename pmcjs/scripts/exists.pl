
my $href = JavaScript->Eval(q| ({ foo: 'fooval', bar: 'barval' }) |);
$href->{'foo'} = 3;
print $href->{'foo'}, "\n";
print exists($href->{'foo'}), "\n";
print ref($href), "\n";

my $aref = JavaScript->Eval('[ 10, 20, 30 ]');
print ref($aref), "\n";
# print $aref->[2], "\n";
# print exists($aref->[3]), "\n";
# my @arr = ( 10, 20, 30 );
# print exists($arr[3]), "\n";

