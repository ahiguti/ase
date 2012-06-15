
use Newt;
Newt::Init();

my $panel = Newt::Panel(2, 3, "Example");
my $entry = Newt::Entry(30, Newt::NEWT_FLAG_RETURNEXIT, "Hello");
my $button = Newt::Button("Ok");
print "$panel\n";
$panel->Add(0, 0, $entry);
print "$panel\n";
$panel->Add(1, 0, $button);
print "$panel\n";
my @r = $panel->Run();

Newt::Finished();
