
my $Form = $CLR->Load("System.Windows.Forms",
  "System.Windows.Forms.Form");
my $Application = $CLR->Load("System.Windows.Forms",
  "System.Windows.Forms.Application");
my $x = $Form->NewInstance();
$x->{Text} = "ASE-CLR bridge";
$x->Show();
$Application->Run($x);

