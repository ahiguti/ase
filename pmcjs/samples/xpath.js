
use("Perl");
Perl.Use("XML::XPath");
Perl.Use("XML::XPath::XMLParser");
var xpfuncs = Perl.Functions['XML::XPath'];
var xp = new Perl("XML::XPath", 'filename', 'domtest.xml');
var nodes = xp.find('/data/foo');
for (var i = 1; i <= nodes.size(); ++i) {
  var node = nodes.get_node(i);
  var txelm = node.getFirstChild();
  print(txelm.getNodeValue());
}

