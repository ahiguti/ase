
local Form = CLR.Load("System.Windows.Forms",
  "System.Windows.Forms.Form");
local Application = CLR.Load("System.Windows.Forms",
  "System.Windows.Forms.Application");
local x = Form();
x.Text = "ASE-CLR bridge";
x.Show();
Application.Run(x)

