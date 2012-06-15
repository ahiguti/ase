
var Form = CLR.Load("System.Windows.Forms",
  "System.Windows.Forms.Form");
var Application = CLR.Load("System.Windows.Forms",
  "System.Windows.Forms.Application");
var x = Form();
x.Text = "Foo";
function on_resize() {
  print("resized ");
}
CLR.AddEventHandler(x, "Resize", on_resize);
x.Show();
Application.Run(x)

