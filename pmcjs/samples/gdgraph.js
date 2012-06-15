
use('Perl');
Perl.Use('GD::Graph::bars');

var data = [
  ["1st","2nd","3rd","4th","5th","6th","7th", "8th", "9th"],
  [    1,    2,    5,    6,    3,  1.5,    1,     3,     4],
  [    1,    1,  1.5,    2,    3,    3,    4,     5,     6]
];

var graph = new Perl('GD::Graph::bars', 400, 300);
graph.set(
  'x_label', 'X Label',
  'y_label', 'Y Label',
  'title', 'Some simple graph',
  'y_max_value', 8,
  'y_tick_number', 8,
  'y_label_skip', 2);

var gd = graph.plot(data);
if (!gd) {
  throw graph.error();
}
var s = gd.png();

Perl.Use('FileHandle');
var fh = new Perl('FileHandle', 'gdgraphout.png', 'w');
fh.binmode();
fh.print(s);
fh.close();
print('wrote gdgraphout.png');

