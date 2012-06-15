
local UIManager = Java['javax.swing.UIManager'];
local JFrame = Java['javax.swing.JFrame']
local JPanel = Java['javax.swing.JPanel']
local JTextArea = Java['javax.swing.JTextArea']
local JTextField = Java['javax.swing.JTextField']
local JLabel = Java['javax.swing.JLabel']
local JButton = Java['javax.swing.JButton']
local JScrollPane = Java['javax.swing.JScrollPane']
local EmptyBorder = Java['javax.swing.border.EmptyBorder']
local Dimension = Java['java.awt.Dimension']
local GridBagLayout = Java['java.awt.GridBagLayout']
local GridBagConstraints = Java['java.awt.GridBagConstraints']
local Insets = Java['java.awt.Insets']
local ActionListener = Java['java.awt.event.ActionListener']

-- local lf = "com.sun.java.swing.plaf.motif.MotifLookAndFeel"
-- local lf = "com.sun.java.swing.plaf.windows.WindowsLookAndFeel";
-- UIManager.setLookAndFeel(lf);

local fr = JFrame()
fr.setBounds(10, 10, 550, 250)
local p = JPanel()
local layout = GridBagLayout()
p.setLayout(layout)
fr.getContentPane().add(p)
p.setBorder(EmptyBorder(5, 5, 5, 5))
fr.setMinimumSize(Dimension(300, 150))

local la = JLabel('Expression')
local tf = JTextArea('', 20, 30)
local sp = JScrollPane(tf)
local la2 = JLabel('Value')
local tf2 = JTextField('', 20)
tf2.setEditable(false)
local lnr = {
  actionPerformed = function(e)
    local tfnc, r, e, estr, src
    src = tf.getText()
    e, estr = pcall(function() r = eval(src) end)
    if not e then r = estr end
    pcall(function() tf2.setText(r) end)
  end
}
local bt = JButton('Evaluate')
bt.addActionListener(ActionListener(lnr))

local gbc = GridBagConstraints()
gbc.insets = Insets(2, 2, 2, 2)

gbc.gridx = 0
gbc.gridy = 0
gbc.gridwidth = 1
gbc.gridheight = 1
gbc.weightx = 0.0
gbc.weighty = 1.0
gbc.fill = GridBagConstraints.VERTICAL
layout.setConstraints(la, gbc)
p.add(la)

gbc.gridx = 1
gbc.gridy = 0
gbc.gridwidth = 1
gbc.gridheight = 1
gbc.weightx = 1.0
gbc.weighty = 1.0
gbc.fill = GridBagConstraints.BOTH
layout.setConstraints(sp, gbc)
p.add(sp)

gbc.gridx = 2
gbc.gridy = 0
gbc.gridwidth = 1
gbc.gridheight = 2
gbc.weightx = 0.0
gbc.weighty = 1.0
gbc.fill = GridBagConstraints.VERTICAL
layout.setConstraints(bt, gbc)
p.add(bt)

gbc.gridx = 0
gbc.gridy = 1
gbc.gridwidth = 1
gbc.gridheight = 1
gbc.weightx = 0.0
gbc.weighty = 0.0
gbc.fill = GridBagConstraints.VERTICAL
layout.setConstraints(la2, gbc)
p.add(la2)

gbc.gridx = 1
gbc.gridy = 1
gbc.gridwidth = 1
gbc.gridheight = 1
gbc.weightx = 0.0
gbc.weighty = 0.0
gbc.fill = GridBagConstraints.HORIZONTAL
layout.setConstraints(tf2, gbc)
p.add(tf2)

fr.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE)
fr.show()

