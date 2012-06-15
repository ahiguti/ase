
use strict;
use Win32::OLE;
my $ex = Win32::OLE->GetActiveObject('Excel.Application')
  || Win32::OLE->new('Excel.Application');
$ex->{Visible} = 1;
$ex->{WorkBooks}->Add();
$ex->Cells(1, 1)->{Value} = 'A';

