
-- Native.Util.Thread.sleep(10000);

local WShell = COM.CreateObject("WScript.Shell");
local Forms = CLR.Load("System.Windows.Forms");
local WebBrowser = Forms.GetType("System.Windows.Forms.WebBrowser");

local wb = WebBrowser();
CLR.AddEventHandler(wb, "Resize", on_resize);
CLR.AddEventHandler(wb, "MouseClick", mouse_click);

