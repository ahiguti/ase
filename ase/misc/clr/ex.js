
// var WShell = COM.CreateObject("WScript.Shell");
var Forms = CLR.Load("System.Windows.Forms");
var WebBrowser = Forms.GetType("System.Windows.Forms.WebBrowser");

var on_resize = { };
var mouse_click = { };
var wb = WebBrowser();
CLR.AddEventHandler(wb, "Resize", on_resize);
for (var i = 0; i < 1000; ++i) {
  try {
  CLR.AddEventHandler(wb, "MouseClick", mouse_click);
  } catch (e) {
  }
}

