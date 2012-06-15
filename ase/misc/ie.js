
var WShell = COM.CreateObject("WScript.Shell");
var ie = COM.CreateObject("InternetExplorer.Application");
ie.Visible = true;
ie.Navigate("about:blank");
while (ie.busy) {
  Native.Util.Thread.sleep(100);
}
while (ie.Document.readyState != "complete") {
  Native.Util.Thread.sleep(100);
}
while (true) {
  Native.Util.Thread.sleep(100);
  print("sleep");
}
print("DONE");

