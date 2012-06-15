
my $Swing = $ASE->{Java}->{javax}->{swing};
my $AWT = $ASE->{Java}->{java}->{awt};

my $fr = $Swing->JFrame();
$fr->setBounds(10, 10, 550, 250);
my $pnl = $Swing->JPanel();
my $layout = $AWT->GridBagLayout();
$pnl->setLayout($layout);
my $cp = $fr->getContentPane();
$cp->add($pnl);
$pnl->setBorder($Swing->{border}->EmptyBorder(5, 5, 5, 5));
$fr->setMinimumSize($AWT->Dimension(300, 150));

my $la = $Swing->JLabel('Expression');
my $tf = $Swing->JTextArea('', 20, 30);
my $sp = $Swing->JScrollPane($tf);
my $la2 = $Swing->JLabel('Value');
my $tf2 = $Swing->JTextField('', 20);
$tf2->setEditable(0);
my $lnr = {
  actionPerformed => sub() {
    my $r = eval($tf->getText());
    $r = $@ if $@;
    $tf2->setText("$r");
  }
};
my $bt = $Swing->JButton('Evaluate');
$bt->addActionListener($AWT->{event}->ActionListener($lnr));

my $gbc = $AWT->GridBagConstraints();
$gbc->{insets} = $AWT->Insets(2, 2, 2, 2);
sub addControl() {
  $gbc->{gridx} = $_[1];
  $gbc->{gridy} = $_[2];
  $gbc->{gridwidth} = $_[3];
  $gbc->{gridheight} = $_[4];
  $gbc->{weightx} = $_[5];
  $gbc->{weighty} = $_[6];
  $gbc->{fill} = $AWT->{GridBagConstraints}->{$_[7]};
  $layout->setConstraints($_[0], $gbc);
  $pnl->add($_[0]);
}

&addControl($la, 0, 0, 1, 1, 0.0, 1.0, "VERTICAL");
&addControl($sp, 1, 0, 1, 1, 1.0, 1.0, "BOTH");
&addControl($bt, 2, 0, 1, 2, 0.0, 1.0, "VERTICAL");
&addControl($la2, 0, 1, 1, 1, 0.0, 0.0, "VERTICAL");
&addControl($tf2, 1, 1, 1, 1, 0.0, 0.0, "HORIZONTAL");

$fr->setDefaultCloseOperation($Swing->{JFrame}->{EXIT_ON_CLOSE});
$fr->show();

