
use("Perl");
Perl.Use("XML::DOM");
Perl.Use("XML::XQL");
Perl.Use("XML::XQL::DOM");
var parser = new Perl("XML::DOM::Parser");
var doc = parser.parsefile("domtest.xml");
var query = new Perl('XML::XQL::Query', 'Expr', 'data/foo');
var result = query.InvokeLC('solve', doc);
var n = result.length;
for (var i = 0; i < n; ++i) {
  var node = result[i];
  var txelm = node.getFirstChild();
  print(txelm.getNodeValue());
}
query.dispose();
doc.dispose();

