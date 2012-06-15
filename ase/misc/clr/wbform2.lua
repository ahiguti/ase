
-- initialize COM
local WShell = COM.CreateObject("WScript.Shell");

local Forms = CLR.Load("System.Windows.Forms");
local Drawing = CLR.Load("System.Drawing");
local Application = Forms.GetType("System.Windows.Forms.Application");
local Form = Forms.GetType("System.Windows.Forms.Form");
local Keys = Forms.GetType("System.Windows.Forms.Keys");
local WebBrowser = Forms.GetType("System.Windows.Forms.WebBrowser");
local ToolStrip = Forms.GetType("System.Windows.Forms.ToolStrip");
local ToolStripButton = Forms.GetType("System.Windows.Forms.ToolStripButton");
local ToolStripTextBox = Forms.GetType(
  "System.Windows.Forms.ToolStripTextBox");
local Size = Drawing.GetType("System.Drawing.Size");
local Point = Drawing.GetType("System.Drawing.Point");
local DockStyle = Forms.GetType("System.Windows.Forms.DockStyle");
local Uri = CLR.Load("system", "System.Uri");

local tstrip = ToolStrip();
local btngo = ToolStripButton();
local tbx = ToolStripTextBox();
local wb = WebBrowser();
local form = Form();

tbx.Size = Size(500, 24);
tstrip.Dock = DockStyle.Top();
btngo.Text = "Go";
tstrip.Items().Add(tbx);
tstrip.Items().Add(btngo);
wb.Dock = DockStyle.Fill();
form.SuspendLayout();
form.ClientSize = Size(600, 400);
form.Controls().Add(wb);
form.Controls().Add(tstrip);
form.ResumeLayout(false);
function on_go_click()
  local r, err = pcall(function() wb.Navigate(Uri(tbx.Text())) end);
  if (not r) then on_navigated() end;
end
function on_navigated()
  local url = wb.Url();
  if (url) then tbx.Text = url.ToString() end;
end
function on_box_keydown(sender, e)
  if (e.KeyCode().Equals(Keys.Enter())) then on_go_click() end;
end
CLR.AddEventHandler(wb, "Navigated", on_navigated);
CLR.AddEventHandler(btngo, "Click", on_go_click);
CLR.AddEventHandler(tbx, "KeyDown", on_box_keydown);

wb.Navigate("about:blank");
Application.Run(form)

