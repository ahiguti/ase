
local Forms = CLR.Load("System.Windows.Forms");
local Form = Forms.GetType("System.Windows.Forms.Form");
local Application = Forms.GetType("System.Windows.Forms.Application");
local form = Form();
form.Text = "ASE-CLR bridge";
function on_resize(sender, ev)
  print("reisize height=" .. sender.Height());
end
function mouse_click(sender, ev)
  print("click x=" .. ev.X() .. " y=" .. ev.Y());
end
CLR.AddEventHandler(form, "Resize", on_resize);
CLR.AddEventHandler(form, "MouseClick", mouse_click);
form.Show();
-- Application.Run(form)
form.Dispose();

