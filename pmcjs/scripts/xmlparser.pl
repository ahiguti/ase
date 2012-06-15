
use XML::Parser;
my $parser = XML::Parser->new();

sub handle_text() {
  print "text: $_[1]\n";
}

my $handle_default = sub {
  print "default: $_[1]\n";
};

$parser->setHandlers('Char', \&handle_text);
$parser->setHandlers('Default', $handle_default);
$parser->parsefile("domtest.xml");

