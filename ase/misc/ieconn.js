
var quit_flag = false;
var cb = {
  'CommandStateChange' : function() {
  },
  'OnQuit' : function() {
    quit_flag = true;
  }
};
var ie = COM.CreateObject("InternetExplorer.Application");
COM.Connect(ie, cb, "DWebBrowserEvents2");
print("IEC1");
ie.Visible = true;
print("IEC2");
ie.Navigate("about:blank");
print("IEC3");
while (ie.Busy) {
  print("IEC4");
  COM.DispatchMessage(100);
  print("IEC5");
}
print("IEC6");
while (ie.Document.ReadyState != "complete") {
  print("IEC7");
  COM.DispatchMessage(100);
  print("IEC8");
}
print("IEC9");
while (!quit_flag) {
  print("IEC10");
  COM.DispatchMessage(100);
  print("IEC11");
}
print("DONE");

