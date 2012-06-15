
use("Perl");
Perl.Use("XML::Parser");
var parser = new Perl("XML::Parser");

function handle_start(expat, elem, attr, val) {
  print("start: [" + elem + "]");
}
function handle_end(expat, elem) {
  print("end: [" + elem + "]");
}
function handle_text(expat, str) {
  print("char: [" + str + "]");
}
function handle_default(expat, str) {
  print("default: [" + str + "]");
}

parser.setHandlers('Char', handle_text);
parser.setHandlers('Start', handle_start);
parser.setHandlers('End', handle_end);
parser.setHandlers('Default', handle_default);
parser.parsefile("domtest.xml");

