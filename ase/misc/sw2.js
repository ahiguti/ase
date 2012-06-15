
var Thread = Java['java.lang.Thread'];
var c = Thread.activeCount();
print('init c=' + c);

var JFrame = Java['javax.swing.JFrame'];

var fr = JFrame.NewInstance();
fr.setBounds(10, 10, 200, 300);
var eoc = JFrame.EXIT_ON_CLOSE;
print("eoc=" + eoc);
fr.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
fr.show();
print("done");

if (0) {
while (true) {
  var c = Thread.activeCount();
  if (c > 1) {
    print('c=' + c);
    Thread.sleep(1000);
  }
}
}

