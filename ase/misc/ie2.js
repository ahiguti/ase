
var cb = {
};
var WShell = COM.CreateObject("WScript.Shell");
var ie = COM.CreateObject("InternetExplorer.Application");
COM.Connect(ie, cb);
ie.Visible = true;
ie.Navigate("about:blank");
while (ie.busy()) {
  COM.Sleep(100);
}
while (ie.Document().readyState() != "complete") {
  COM.Sleep(100);
}
while (true) {
  COM.Sleep(100);
  print("sleep");
}
print("DONE");

