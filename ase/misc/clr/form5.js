
var Form = CLR.Load("System.Windows.Forms",
  "System.Windows.Forms.Form");
var Application = CLR.Load("System.Windows.Forms",
  "System.Windows.Forms.Application");
var x = Form();
var obj = {};
x = CLR.Extend(x, obj);
obj.OnResize = function() {
  print("resized");
}
x.Show();
Application.Run(x)

