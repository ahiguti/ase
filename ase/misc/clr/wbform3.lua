
-- initialize COM
local WShell = COM.CreateObject("WScript.Shell");

local Forms = CLR.Load("System.Windows.Forms", "System.Windows.Forms");
local Drawing = CLR.Load("System.Drawing", "System.Drawing");
local Uri = CLR.Load("System", "System.Uri");

local anc_top = Forms.AnchorStyles.Top;
local anc_bottom = Forms.AnchorStyles.Bottom;
local anc_left = Forms.AnchorStyles.Left;
local anc_right = Forms.AnchorStyles.Right;

local btngo = Forms.Button();
local tbx = Forms.TextBox();
local wb = Forms.WebBrowser();
local form = Forms.Form();

form.SuspendLayout();
form.ClientSize = Drawing.Size(400, 400);
tbx.Location = Drawing.Point(5, 5);
tbx.Size = Drawing.Size(350, 20);
tbx.Anchor = CLR.EnumOr(anc_top, anc_left, anc_right);
btngo.Text = "Go";
btngo.Location = Drawing.Point(360, 5);
btngo.Size = Drawing.Size(35, 20);
btngo.Anchor = CLR.EnumOr(anc_top, anc_right);
wb.Location = Drawing.Point(0, 30);
wb.Size = Drawing.Size(400, 370);
wb.Anchor = CLR.EnumOr(anc_top, anc_bottom, anc_left, anc_right);
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
  if (e.KeyCode.Equals(Forms.Keys.Enter)) then on_go_click() end;
end
CLR.AddEventHandler(wb, "Navigated", on_navigated);
CLR.AddEventHandler(btngo, "Click", on_go_click);
CLR.AddEventHandler(tbx, "KeyDown", on_box_keydown);

wb.Navigate("about:blank");
Forms.Application.Run(form)

