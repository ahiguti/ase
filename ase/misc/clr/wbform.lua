
-- initialize COM
local WShell = COM.CreateObject("WScript.Shell");

local Forms = CLR.Load("System.Windows.Forms");
local Drawing = CLR.Load("System.Drawing");
local Application = Forms.GetType("System.Windows.Forms.Application");
local Form = Forms.GetType("System.Windows.Forms.Form");
local Keys = Forms.GetType("System.Windows.Forms.Keys");
local WebBrowser = Forms.GetType("System.Windows.Forms.WebBrowser");
local AnchorStyles = Forms.GetType("System.Windows.Forms.AnchorStyles");
local Button = Forms.GetType("System.Windows.Forms.Button");
local TextBox = Forms.GetType("System.Windows.Forms.TextBox");
local Size = Drawing.GetType("System.Drawing.Size");
local Point = Drawing.GetType("System.Drawing.Point");
local DockStyle = Forms.GetType("System.Windows.Forms.DockStyle");
local Uri = CLR.Load("system", "System.Uri");

local btngo = Button();
local tbx = TextBox();
local wb = WebBrowser();
local form = Form();

form.SuspendLayout();
form.ClientSize = Size(400, 400);
tbx.Location = Point(5, 5);
tbx.Size = Size(350, 20);
tbx.Anchor = CLR.EnumOr(AnchorStyles.Top, AnchorStyles.Left,
  AnchorStyles.Right);
btngo.Text = "Go";
btngo.Location = Point(360, 5);
btngo.Size = Size(35, 20);
btngo.Anchor = CLR.EnumOr(AnchorStyles.Top, AnchorStyles.Right);
wb.Location = Point(0, 30);
wb.Size = Size(400, 370);
wb.Anchor = CLR.EnumOr(AnchorStyles.Top, AnchorStyles.Bottom,
  AnchorStyles.Left, AnchorStyles.Right);
form.Controls.Add(tbx);
form.Controls.Add(btngo);
form.Controls.Add(wb);
form.ResumeLayout(false);

function on_go_click()
  local r, err = pcall(function() wb.Navigate(Uri(tbx.Text)) end);
  if (not r) then on_navigated() end;
end
function on_navigated()
  local url = wb.Url;
  if (url) then tbx.Text = url.ToString() end;
  form.Text = wb.DocumentTitle;
end
function on_box_keydown(sender, e)
  if (e.KeyCode.Equals(Keys.Enter)) then on_go_click() end;
end
CLR.AddEventHandler(wb, "Navigated", on_navigated);
CLR.AddEventHandler(btngo, "Click", on_go_click);
CLR.AddEventHandler(tbx, "KeyDown", on_box_keydown);

wb.Navigate("about:blank");
Application.Run(form)

