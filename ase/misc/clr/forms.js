
var Form = CLR.Load("System.Windows.Forms",
  "System.Windows.Forms.Form");
var Application = CLR.Load("System.Windows.Forms",
  "System.Windows.Forms.Application");
var x = Form();
x.Show();
Application.Run(x)

