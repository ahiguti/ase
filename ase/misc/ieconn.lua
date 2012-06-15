
local cb = {
  CommandStateChange = function()
  end
}
local ie = COM.CreateObject("InternetExplorer.Application")
COM.Connect(ie, cb, "DWebBrowserEvents2")
ie.Visible = true
ie.Navigate("about:blank")
while (true) do
  Native.Util.Thread.sleep(100)
  print("sleep")
end
print("DONE");

