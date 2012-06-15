
var JFrame = Java['javax.swing.JFrame'];
var JTextField = Java['javax.swing.JTextField'];
var JPanel = Java['javax.swing.JPanel'];

var fr = JFrame.NewInstance();
fr.setBounds(10, 10, 200, 300);
var tf = JTextField.NewInstance("Test", 15);
var p = JPanel.NewInstance();
p.add(tf);
fr.getContentPane().add(p);

fr.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
fr.show();

