
var JFrame = Java['javax.swing.JFrame'];
var JPanel = Java['javax.swing.JPanel'];
var JTextArea = Java['javax.swing.JTextArea'];
var JTextField = Java['javax.swing.JTextField'];
var JLabel = Java['javax.swing.JLabel'];
var JButton = Java['javax.swing.JButton'];
var JScrollPane = Java['javax.swing.JScrollPane'];
var EmptyBorder = Java['javax.swing.border.EmptyBorder'];
var Dimension = Java['java.awt.Dimension'];
var GridBagLayout = Java['java.awt.GridBagLayout'];
var GridBagConstraints = Java['java.awt.GridBagConstraints'];
var Insets = Java['java.awt.Insets'];

var fr = JFrame.NewInstance();
fr.setBounds(10, 10, 550, 250);
var p = JPanel.NewInstance();
var layout = GridBagLayout.NewInstance();
p.setLayout(layout);
fr.getContentPane().add(p);
p.setBorder(EmptyBorder.NewInstance(5, 5, 5, 5));
fr.setMinimumSize(Dimension.NewInstance(300, 150));

var la = JLabel.NewInstance('Expression');
var tf = JTextArea.NewInstance('', 20, 30);
var sp = JScrollPane.NewInstance(tf);
var la2 = JLabel.NewInstance('Value');
var tf2 = JTextField.NewInstance('', 20);
tf2.setEditable(false);
var lnr = {
  actionPerformed : function(e) {
    tf2.setText(String(eval(tf.getText())));
  }
};
var bt = JButton.NewInstance('Evaluate');
bt.addActionListener(lnr);

var gbc = GridBagConstraints.NewInstance();
gbc.insets = Insets.NewInstance(2, 2, 2, 2);

gbc.gridx = 0;
gbc.gridy = 0;
gbc.gridwidth = 1;
gbc.gridheight = 1;
gbc.weightx = 0.0;
gbc.weighty = 1.0;
gbc.fill = GridBagConstraints.VERTICAL;
layout.setConstraints(la, gbc);
p.add(la);

gbc.gridx = 1;
gbc.gridy = 0;
gbc.gridwidth = 1;
gbc.gridheight = 1;
gbc.weightx = 1.0;
gbc.weighty = 1.0;
gbc.fill = GridBagConstraints.BOTH;
layout.setConstraints(sp, gbc);
p.add(sp);

gbc.gridx = 2;
gbc.gridy = 0;
gbc.gridwidth = 1;
gbc.gridheight = 2;
gbc.weightx = 0.0;
gbc.weighty = 1.0;
gbc.fill = GridBagConstraints.VERTICAL;
layout.setConstraints(bt, gbc);
p.add(bt);

gbc.gridx = 0;
gbc.gridy = 1;
gbc.gridwidth = 1;
gbc.gridheight = 1;
gbc.weightx = 0.0;
gbc.weighty = 0.0;
gbc.fill = GridBagConstraints.VERTICAL;
layout.setConstraints(la2, gbc);
p.add(la2);

gbc.gridx = 1;
gbc.gridy = 1;
gbc.gridwidth = 1;
gbc.gridheight = 1;
gbc.weightx = 0.0;
gbc.weighty = 0.0;
gbc.fill = GridBagConstraints.HORIZONTAL;
layout.setConstraints(tf2, gbc);
p.add(tf2);

fr.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
fr.show();

