
var font = '/usr/share/fonts/japanese/TrueType/ipagui.ttf';
// var font = 'c:/windows/fonts/MSUIgothic.ttf';

use('Perl');
Perl.Use('GD::Graph::lines');
Perl.Use('GD::Graph::bars');
Perl.Use('GD::Graph::hbars');
Perl.Use('GD::Graph::points');
Perl.Use('GD::Graph::linespoints');
Perl.Use('GD::Graph::area');
Perl.Use('GD::Graph::mixed');
Perl.Use('GD::Graph::pie');
Perl.Use('GD::Graph::pie3d');
Perl.Use('GD::Graph::bars3d');
Perl.Use('GD::Graph::lines3d');
Perl.Use('FileHandle');

function draw_graph(gi) {
  var graph = new Perl('GD::Graph::' + gi.klass, 500, 400);
  var props = gi.props;
  for (var i = 0; i < props.length; i += 2) {
    graph.set(props[i], props[i + 1]);
  }
  gi.setfont(graph);
  var gd = graph.plot(gi.data);
  if (!gd) {
    throw graph.error();
  }
  var s = gd.png();
  var fn = 'gdgraphuc2-' + gi.klass + '.png';
  var h = new Perl('FileHandle', fn, 'w');
  h.binmode();
  h.print(s);
  h.close();
  print('wrote ' + fn);
}

var data1 = [
  ["1st","2nd","3rd","4th","5th","6th","7th", "8th", "9th"],
  [    1,    2,    5,    6,    3,  1.5,    1,     3,     4],
  [    1,    1,  1.5,    2,    3,    3,    4,     5,     6]
];

var props1 = [
  'x_label', 'X 軸',
  'y_label', 'Y 軸',
  'title', '単純なグラフ',
  'y_max_value', 8,
  'y_tick_number', 8,
  'y_label_skip', 2
];

var props2 = [
  'title', '単純なグラフ',
];

var set_font1 = function(graph) {
  graph.set_title_font(font, 10);
  graph.set_x_label_font(font, 10);
  graph.set_y_label_font(font, 10);
};

var set_font2 = function(graph) {
  graph.set_title_font(font, 10);
};

var ginfo = [
  { klass: 'lines', props: props1, setfont: set_font1, data: data1 },
  { klass: 'bars', props: props1, setfont: set_font1, data: data1 },
  { klass: 'hbars', props: props1, setfont: set_font1, data: data1 },
  { klass: 'points', props: props1, setfont: set_font1, data: data1 },
  { klass: 'linespoints', props: props1, setfont: set_font1, data: data1 },
  { klass: 'area', props: props1, setfont: set_font1, data: data1 },
  { klass: 'mixed', props: props1, setfont: set_font1, data: data1 },
  { klass: 'lines3d', props: props1, setfont: set_font1, data: data1 },
  { klass: 'bars3d', props: props1, setfont: set_font1, data: data1 },
  { klass: 'pie', props: props2, setfont: set_font2, data: data1 },
  { klass: 'pie3d', props: props2, setfont: set_font2, data: data1 }
];

for (var i = 0; i < ginfo.length; ++i) {
  draw_graph(ginfo[i]);
}

