
local AWT = Java.java.awt;
local Swing = Java.javax.swing;

local fr = Swing.JFrame("Swing - Lua")
fr.setBounds(10, 10, 550, 250)
local p = Swing.JPanel()
local layout = AWT.GridBagLayout()
p.setLayout(layout)
fr.getContentPane().add(p)
p.setBorder(Swing.border.EmptyBorder(5, 5, 5, 5))
fr.setMinimumSize(AWT.Dimension(300, 150))

local la = Swing.JLabel('Expression')
local tf = Swing.JTextArea('', 20, 30)
local sp = Swing.JScrollPane(tf)
local la2 = Swing.JLabel('Value')
local tf2 = Swing.JTextField('', 20)
tf2.setEditable(false)
local lnr = {
  actionPerformed = function(e)
    local tfnc, r, e, estr, src
    src = tf.getText()
    tfnc = function()
      r = EvalString(src)
    end
    e, estr = pcall(tfnc)
    if not e then r = estr end
    tf2.setText(r)
  end
}
local bt = Swing.JButton('Evaluate')
bt.addActionListener(AWT.event.ActionListener(lnr))

local gbc = AWT.GridBagConstraints()
gbc.insets = AWT.Insets(2, 2, 2, 2)
local ctrls = {
  { la, 0, 0, 1, 1, 0.0, 1.0, 'VERTICAL' },
  { sp, 1, 0, 1, 1, 1.0, 1.0, 'BOTH' },
  { bt, 2, 0, 1, 2, 0.0, 1.0, 'VERTICAL' },
  { la2, 0, 1, 1, 1, 0.0, 0.0, 'VERTICAL' },
  { tf2, 1, 1, 1, 1, 0.0, 0.0, 'HORIZONTAL' },
};
local i, v
for i, v in pairs(ctrls) do
  gbc.gridx = v[2]
  gbc.gridy = v[3]
  gbc.gridwidth = v[4]
  gbc.gridheight = v[5]
  gbc.weightx = v[6]
  gbc.weighty = v[7]
  gbc.fill = AWT.GridBagConstraints[v[8]]
  layout.setConstraints(v[1], gbc)
  p.add(v[1])
end

fr.setDefaultCloseOperation(Swing.JFrame.EXIT_ON_CLOSE)
fr.show()

