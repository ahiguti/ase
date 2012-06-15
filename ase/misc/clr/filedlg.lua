
local Forms = CLR.LoadAssembly("System.Windows.Forms");
local OpenFileDialog = Forms.GetType("System.Windows.Forms.OpenFileDialog");
local MessageBox = Forms.GetType("System.Windows.Forms.MessageBox");
local DialogResult = Forms.GetType("System.Windows.Forms.DialogResult");
local dlg = OpenFileDialog.NewInstance();
dlg.Title = "Test";
local re = dlg.ShowDialog();

if (re.Equals(DialogResult.OK())) then
  MessageBox.Show(dlg.FileName());
end

