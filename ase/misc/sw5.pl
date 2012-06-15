
my $JFrame = $Java->{'javax.swing.JFrame'};
my $JPanel = $Java->{'javax.swing.JPanel'};
my $JTextArea = $Java->{'javax.swing.JTextArea'};
my $JTextField = $Java->{'javax.swing.JTextField'};
my $JLabel = $Java->{'javax.swing.JLabel'};
my $JButton = $Java->{'javax.swing.JButton'};
my $JScrollPane = $Java->{'javax.swing.JScrollPane'};
my $EmptyBorder = $Java->{'javax.swing.border.EmptyBorder'};
my $Dimension = $Java->{'java.awt.Dimension'};
my $GridBagLayout = $Java->{'java.awt.GridBagLayout'};
my $GridBagConstraints = $Java->{'java.awt.GridBagConstraints'};
my $Insets = $Java->{'java.awt.Insets'};

my $fr = $JFrame->NewInstance();
$fr->setBounds(10, 10, 550, 250);
my $p = $JPanel->NewInstance();
my $layout = $GridBagLayout->NewInstance();
$p->setLayout($layout);
my $cp = $fr->getContentPane();
$cp->add($p);
$p->setBorder($EmptyBorder->NewInstance(5, 5, 5, 5));
$fr->setMinimumSize($Dimension->NewInstance(300, 150));

my $la = $JLabel->NewInstance('Expression');
my $tf = $JTextArea->NewInstance('', 20, 30);
my $sp = $JScrollPane->NewInstance($tf);
my $la2 = $JLabel->NewInstance('Value');
my $tf2 = $JTextField->NewInstance('', 20);
$tf2->setEditable(0);
my $lnr = {
  actionPerformed => sub() {
    my $r = eval($tf->getText());
    $tf2->setText("$r");
  }
};
my $bt = $JButton->NewInstance('Evaluate');
$bt->addActionListener($lnr);

my $gbc = $GridBagConstraints->NewInstance();
$gbc->{insets} = $Insets->NewInstance(2, 2, 2, 2);

$gbc->{gridx} = 0;
$gbc->{gridy} = 0;
$gbc->{gridwidth} = 1;
$gbc->{gridheight} = 1;
$gbc->{weightx} = 0;
$gbc->{weighty} = 1;
$gbc->{fill} = $GridBagConstraints->{VERTICAL};
$layout->setConstraints($la, $gbc);
$p->add($la);

$gbc->{gridx} = 1;
$gbc->{gridy} = 0;
$gbc->{gridwidth} = 1;
$gbc->{gridheight} = 1;
$gbc->{weightx} = 1;
$gbc->{weighty} = 1;
$gbc->{fill} = $GridBagConstraints->{BOTH};
$layout->setConstraints($sp, $gbc);
$p->add($sp);

$gbc->{gridx} = 2;
$gbc->{gridy} = 0;
$gbc->{gridwidth} = 1;
$gbc->{gridheight} = 2;
$gbc->{weightx} = 0;
$gbc->{weighty} = 1;
$gbc->{fill} = $GridBagConstraints->{VERTICAL};
$layout->setConstraints($bt, $gbc);
$p->add($bt);

$gbc->{gridx} = 0;
$gbc->{gridy} = 1;
$gbc->{gridwidth} = 1;
$gbc->{gridheight} = 1;
$gbc->{weightx} = 0;
$gbc->{weighty} = 0;
$gbc->{fill} = $GridBagConstraints->{VERTICAL};
$layout->setConstraints($la2, $gbc);
$p->add($la2);

$gbc->{gridx} = 1;
$gbc->{gridy} = 1;
$gbc->{gridwidth} = 1;
$gbc->{gridheight} = 1;
$gbc->{weightx} = 0;
$gbc->{weighty} = 0;
$gbc->{fill} = $GridBagConstraints->{HORIZONTAL};
$layout->setConstraints($tf2, $gbc);
$p->add($tf2);

$fr->setDefaultCloseOperation($JFrame->{EXIT_ON_CLOSE});
$fr->show();

