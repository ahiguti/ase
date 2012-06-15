
use('Perl');
Perl.Use('Win32::OLE');
var OLE = Perl.Methods['Win32::OLE'];
var ex = OLE.GetActiveObject('Excel.Application')
  || new Perl('Win32::OLE', 'Excel.Application');
ex.Visible = 1;
ex.WorkBooks.Add;
var items = ex.Cells.Item;
items(1, 1).Value = 'A';
items(2, 1).Value = 'B';
items(3, 1).Value = 'C';
items(4, 1).Value = 'Sum';
items(1, 2).Value = '10';
items(2, 2).Value = '20';
items(3, 2).Value = '30';
items(4, 2).Value = '=sum(B1:B3)';
ex.Range('A1:B3').Select;
ex.Charts.Add;

