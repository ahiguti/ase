
# initialize COM
my $WShell = $ASE->{COM}->CreateObject("WScript.Shell");

my $Forms = $ASE->{CLR}->Load("System.Windows.Forms", "System.Windows.Forms");
my $Drawing = $ASE->{CLR}->Load("System.Drawing", "System.Drawing");
my $Uri = $ASE->{CLR}->Load("System", "System.Uri");

# Perl has no boolean type. The followings are alternatives to boolean values.
my $Boolean = $ASE->{CLR}->GetCore()->{System}->{Boolean};
my $False = $ASE->{CLR}->CreateWrapper($Boolean, "false");
my $True = $ASE->{CLR}->CreateWrapper($Boolean, "true");

my $anc_top = $Forms->{AnchorStyles}->{Top};
my $anc_bottom = $Forms->{AnchorStyles}->{Bottom};
my $anc_left = $Forms->{AnchorStyles}->{Left};
my $anc_right = $Forms->{AnchorStyles}->{Right};

my $btngo = $Forms->Button();
my $tbx = $Forms->TextBox();
my $wb = $Forms->WebBrowser();
my $form = $Forms->Form();

$form->SuspendLayout();
$form->{ClientSize} = $Drawing->Size(400, 400);
$tbx->{Location} = $Drawing->Point(5, 5);
$tbx->{Size} = $Drawing->Size(350, 20);
$tbx->{Anchor} = $ASE->{CLR}->EnumOr($anc_top, $anc_left, $anc_right);
$btngo->{Text} = "Go";
$btngo->{Location} = $Drawing->Point(360, 5);
$btngo->{Size} = $Drawing->Size(35, 20);
$btngo->{Anchor} = $ASE->{CLR}->EnumOr($anc_top, $anc_right);
$wb->{Location} = $Drawing->Point(0, 30);
$wb->{Size} = $Drawing->Size(400, 370);
$wb->{Anchor} = $ASE->{CLR}->EnumOr($anc_top, $anc_bottom, $anc_left, $anc_right);
$form->{Controls}->Add($tbx);
$form->{Controls}->Add($btngo);
$form->{Controls}->Add($wb);
$form->ResumeLayout($False);

sub on_go_click() {
  my $r = $wb->Navigate($Uri->InvokeSelf($tbx->{Text}));
  if ($r) { on_navigated(); }
}
sub on_navigated() {
  my $url = $wb->{Url};
  if ($url ne "") { $tbx->{Text} = $url->ToString(); }
  $form->{Text} = $wb->{DocumentTitle};
}
sub on_box_keydown() {
  if ($_[1]->{KeyCode}->Equals($Forms->{Keys}->{Enter})) {
    on_go_click();
  }
}
$ASE->{CLR}->AddEventHandler($wb, "Navigated", \&on_navigated);
$ASE->{CLR}->AddEventHandler($btngo, "Click", \&on_go_click);
$ASE->{CLR}->AddEventHandler($tbx, "KeyDown", \&on_box_keydown);

$wb->Navigate("about:blank");
$Forms->{Application}->Run($form)

