
// Java = GCJ;

var JFrame = Java['javax.swing.JFrame'];
var JPanel = Java['javax.swing.JPanel'];
var JTextField = Java['javax.swing.JTextField'];
var JLabel = Java['javax.swing.JLabel'];
var JButton = Java['javax.swing.JButton'];
var GridLayout = Java['java.awt.GridLayout'];

var fr = JFrame.NewInstance();
fr.setBounds(10, 10, 250, 100);
var p = JPanel.NewInstance();
p.setLayout(GridLayout.NewInstance(2, 3));
fr.getContentPane().add(p);
var la = JLabel.NewInstance('expression');
var tf = JTextField.NewInstance('', 30);
var la2 = JLabel.NewInstance('value');
var tf2 = JTextField.NewInstance('', 20);
tf2.setEditable(false);
var lnr = {
  actionPerformed : function(e) {
    tf2.setText(String(eval(tf.getText())));
  }
};
var bt = JButton.NewInstance('eval');
bt.addActionListener(lnr);
p.add(la);
p.add(tf);
p.add(bt);
p.add(la2);
p.add(tf2);

fr.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
fr.show();

