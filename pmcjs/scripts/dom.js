
use('Perl');
Perl.Use("XML::DOM");
var parser = new Perl("XML::DOM::Parser");
var doc = parser.parsefile("domtest.xml");
var nodes = doc.getElementsByTagName("foo");
var n = nodes.getLength();
for (var i = 0; i < n; ++i) {
  var node = nodes.item(i);
  var txelm = node.getFirstChild();
  print(txelm.getNodeValue());
}
print(doc);
doc.dispose();

