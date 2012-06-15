
/* SKIP_CHECKMEM */

use('Perl');
Perl.Use('Newt');
var newt = Perl.Functions['Newt'];
var newtc = Perl.Constants['Newt'];

newt.Init();
try {
  var panel = newt.Panel(2, 3, "Hello Panel");
  var entry = newt.Entry(30, newtc.NEWT_FLAG_RETURNEXIT, "Hello, World");
  var button = newt.Button("Ok");
  panel.Add(0, 0, entry);
  panel.Add(1, 0, button);
  var r = panel.InvokeLC('Run');
  var reason = r[0];
  var data = r[1];
} finally {
  newt.Finished();
}
