
use('Perl');

function DOMDocument(x) {
  return x.QueryInterface(Components.interfaces.nsIDOMDocument);
}
function DOMNode(x) {
  return x.QueryInterface(Components.interfaces.nsIDOMNode);
}
function DOMElement(x) {
  return x.QueryInterface(Components.interfaces.nsIDOMElement);
}

var doc = DOMDocument(Components.classes["@mozilla.org/xul/xul-document;1"]
  .createInstance());
var x = DOMNode(doc.createElement("top"));
doc.appendChild(x);
x.appendChild(DOMNode(doc.createElement("foo")));
x.appendChild(DOMNode(doc.createElement("bar")));
x.appendChild(DOMNode(doc.createElement("baz")));
for (var i = x.firstChild; i; i = i.nextSibling) {
  var n = DOMElement(i).tagName;
  print(n);
}

