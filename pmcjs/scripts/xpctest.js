
use('Perl');

var lf = Components.classes["@mozilla.org/file/local;1"].createInstance();
lf = lf.QueryInterface(Components.interfaces.nsILocalFile);
lf.initWithPath("/usr/bin");
var ents = lf.directoryEntries;
while (ents.hasMoreElements()) {
  var ent = ents.getNext().QueryInterface(Components.interfaces.nsILocalFile);
  print(ent.leafName);
}
