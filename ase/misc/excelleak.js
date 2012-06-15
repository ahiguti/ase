
var ex = COM.CreateObject("Excel.Application");
ex.Workbooks.Add;
var c = ex.Workbooks.Count;
print("count=" + c);

