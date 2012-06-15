
use('Perl');
Perl.Use('Spreadsheet::ParseExcel');
Perl.Use('Spreadsheet::ParseExcel::FmtJapan');
var ex = new Perl('Spreadsheet::ParseExcel');
var fmt = new Perl('Spreadsheet::ParseExcel::FmtJapan', 'Code', 'utf8');
var book = ex.Parse('sample.xls', fmt);
print("file: " + book.File);
print("sheetcount: " + book.SheetCount);
print("author: " + book.Author);

for (var i = 0; i < book.SheetCount; ++i) {
  var sh = book.Worksheet[i];
  print("--- sheet: " + sh.Name);
  for (var row = sh.MinRow; sh.MaxRow && row <= sh.MaxRow; ++row) {
    for (var col = sh.MinCol; sh.MaxCol && col <= sh.MaxCol; ++col) {
      var cell = sh.Cells[row][col];
      if (!cell) {
	continue;
      }
      var val_formatted = Perl.SuppressConv(
	function() { return cell.Value(); } ).GetText();
      print("[" + row + "," + col + "] [" + val_formatted + "]");
    }
  }
}

