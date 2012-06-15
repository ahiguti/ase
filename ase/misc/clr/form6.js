
var Forms = CLR.LoadAssembly("System.Windows.Forms");
var Form = Forms.GetType("System.Windows.Forms.Form");
var Application = Forms.GetType("System.Windows.Forms.Application");
var x = Form();
x.Text = "Foo";
function on_resize() {
  print("resized");
}
CLR.AddEventHandler(x, "Resize", on_resize);
x.Show();
Application.Run(x)

