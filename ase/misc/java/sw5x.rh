
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
var ActionListener = Java['java.awt.event.ActionListener'];

var fr = JFrame();
fr.setBounds(10, 10, 550, 250);
var p = JPanel();
var layout = GridBagLayout();
p.setLayout(layout);
fr.getContentPane().add(p);
p.setBorder(EmptyBorder(5, 5, 5, 5));
fr.setMinimumSize(Dimension(300, 150));

var la = JLabel('Expression');
var tf = JTextArea('', 20, 30);
var sp = JScrollPane(tf);
var la2 = JLabel('Value');
var tf2 = JTextField('', 20);
tf2.setEditable(false);
var lnr = {
  actionPerformed : function(e) {
    var r;
    try {
      r = EvalString(String(tf.getText()));
    } catch (e) {
      r = e;
    }
    tf2.setText(String(r));
  }
};
var bt = JButton('Evaluate');
bt.addActionListener(ActionListener(lnr));

var gbc = GridBagConstraints();
gbc.insets = Insets(2, 2, 2, 2);

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

