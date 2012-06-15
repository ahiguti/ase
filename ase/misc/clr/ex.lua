
-- local WShell = COM.CreateObject("WScript.Shell");
local Forms = CLR.Load("System.Windows.Forms");
local WebBrowser = Forms.GetType("System.Windows.Forms.WebBrowser");

local wb = WebBrowser();
CLR.AddEventHandler(wb, "Resize", on_resize);
function f() 
  CLR.AddEventHandler(wb, "MouseClick", mouse_click);
end
local err, r;
local i;
for i = 1, 100 do
  r, err = pcall(f);
  print(err);
end

