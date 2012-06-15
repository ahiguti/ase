
my $JFrame = $Java->{'javax.swing.JFrame'};
my $JPanel = $Java->{'javax.swing.JPanel'};
my $JTextField = $Java->{'javax.swing.JTextField'};
my $JLabel = $Java->{'javax.swing.JLabel'};
my $JButton = $Java->{'javax.swing.JButton'};
my $GridLayout = $Java->{'java.awt.GridLayout'};

my $fr = $JFrame->NewInstance();
$fr->setBounds(10, 10, 250, 100);
my $p = $JPanel->NewInstance();
$p->setLayout($GridLayout->NewInstance(2, 3));
$fr->getContentPane()->add($p);
my $la = $JLabel->NewInstance('expression');
my $tf = $JTextField->NewInstance('', 30);
my $la2 = $JLabel->NewInstance('value');
my $tf2 = $JTextField->NewInstance('', 20);
$tf2->setEditable(0);
my $lnr = {
  actionPerformed => sub(e) {
    my $r = eval($tf->getText());
    $tf2->setText("$r");
  }
};
my $bt = $JButton->NewInstance('eval');
$bt->addActionListener($lnr);
$p->add($la);
$p->add($tf);
$p->add($bt);
$p->add($la2);
$p->add($tf2);

$fr->setDefaultCloseOperation($JFrame->{'EXIT_ON_CLOSE'});
$fr->show();

