
use utf8;

my $JFrame = $main::Java->{'javax.swing.JFrame'};
my $JTextField = $main::Java->{'javax.swing.JTextField'};
my $JPanel = $main::Java->{'javax.swing.JPanel'};

my $fr = $JFrame->NewInstance();
$fr->setBounds(10, 10, 200, 300);
my $tf = $JTextField->NewInstance("Test", 15);
my $p = $JPanel->NewInstance();
$p->add($tf);
$fr->getContentPane()->add($p);
$fr->setTitle('ふががが');

$fr->setDefaultCloseOperation($JFrame->{'EXIT_ON_CLOSE'});
$fr->show();

