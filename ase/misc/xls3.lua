
local ex = COM.CreateObject("Excel.Application");
pcall(
function()
  ex.Visible = true;
  ex.WorkBooks().Add();
  ex.Cells(1, 1).Value = "A";
  ex.Cells(2, 1).Value = "B";
  ex.Cells(3, 1).Value = "C";
  ex.Cells(4, 1).Value = "Sum";
  ex.Cells(1, 2).Value = "10";
  ex.Cells(2, 2).Value = "20";
  ex.Cells(3, 2).Value = "30";
  ex.Cells(4, 2).Value = "=sum(B1:B3)";
  ex.Range("A1:C3").Select();
  ex.Charts().Add();
end
)
ex.Quit();

