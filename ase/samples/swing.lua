
local Swing = ASE.Java.javax.swing
local AWT = ASE.Java.java.awt

local fr = Swing.JFrame()
fr.setBounds(10, 10, 550, 250)
local pnl = Swing.JPanel()
local layout = AWT.GridBagLayout()
pnl.setLayout(layout)
fr.getContentPane().add(pnl)
pnl.setBorder(Swing.border.EmptyBorder(5, 5, 5, 5))
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
function addControl(c, gx, gy, gw, gh, wx, wy, fi)
  gbc.gridx = gx
  gbc.gridy = gy
  gbc.gridwidth = gw
  gbc.gridheight = gh
  gbc.weightx = wx
  gbc.weighty = wy
  gbc.fill = AWT.GridBagConstraints[fi]
  layout.setConstraints(c, gbc)
  pnl.add(c)
end

addControl(la, 0, 0, 1, 1, 0.0, 1.0, "VERTICAL")
addControl(sp, 1, 0, 1, 1, 1.0, 1.0, "BOTH")
addControl(bt, 2, 0, 1, 2, 0.0, 1.0, "VERTICAL")
addControl(la2, 0, 1, 1, 1, 0.0, 0.0, "VERTICAL")
addControl(tf2, 1, 1, 1, 1, 0.0, 0.0, "HORIZONTAL")

fr.setDefaultCloseOperation(Swing.JFrame.EXIT_ON_CLOSE)
fr.show()

