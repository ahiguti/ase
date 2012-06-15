
var MessageBox = CLR.Load("System.Windows.Forms",
  "System.Windows.Forms.MessageBox");
print(String(MessageBox));

var Swing = Java.javax.swing;
var AWT = Java.java.awt;

var fr = Swing.JFrame();
fr.setBounds(10, 10, 550, 250);
var pnl = Swing.JPanel();
var layout = AWT.GridBagLayout();
pnl.setLayout(layout);
fr.getContentPane().add(pnl);
pnl.setBorder(Swing.border.EmptyBorder(5, 5, 5, 5));
fr.setMinimumSize(AWT.Dimension(300, 150));

var la = Swing.JLabel('Expression');
var tf = Swing.JTextArea('', 20, 30);
var sp = Swing.JScrollPane(tf);
var la2 = Swing.JLabel('Value');
var tf2 = Swing.JTextField('', 20);
tf2.setEditable(false);
var lnr = {
  actionPerformed : function(e) {
    var r;
    try {
      r = eval(String(tf.getText()));
      // MessageBox.Show(String(r));
    } catch (e) {
      r = e;
    }
    // MessageBox.Show(r);
    tf2.setText(String(r));
  }
};
var bt = Swing.JButton('Evaluate');
bt.addActionListener(AWT.event.ActionListener(lnr));

var gbc = AWT.GridBagConstraints();
gbc.insets = AWT.Insets(2, 2, 2, 2);
function addControl(c, gx, gy, gw, gh, wx, wy, fi) {
  gbc.gridx = gx;
  gbc.gridy = gy;
  gbc.gridwidth = gw;
  gbc.gridheight = gh;
  gbc.weightx = wx;
  gbc.weighty = wy;
  gbc.fill = AWT.GridBagConstraints[fi];
  layout.setConstraints(c, gbc);
  pnl.add(c);
}

addControl(la, 0, 0, 1, 1, 0.0, 1.0, "VERTICAL");
addControl(sp, 1, 0, 1, 1, 1.0, 1.0, "BOTH");
addControl(bt, 2, 0, 1, 2, 0.0, 1.0, "VERTICAL");
addControl(la2, 0, 1, 1, 1, 0.0, 0.0, "VERTICAL");
addControl(tf2, 1, 1, 1, 1, 0.0, 0.0, "HORIZONTAL");

fr.setDefaultCloseOperation(Swing.JFrame.EXIT_ON_CLOSE);
fr.show();

