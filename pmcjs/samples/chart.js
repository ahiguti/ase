
/* copied from an exaple in perl-Chart-2.4.1/Documentation.pdf */

use('Perl');
Perl.Use('Chart::LinesPoints');

var labels = [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 ];
var data1 = [ -7,-5,-6,-8,-9,-7,-5,-4,-3,-2,-4,-6,-3,-5,-3,-4,-6 ];
var data2 = [ -1,-1,-1,-1,-2,-2,-3,-3,-4,-4,-6,-3,-2,-2,-2,-1,-1 ];
var data3 = [ -4,-4,-3,-2,-1,-1,-1,-2,-1,-1,-3,-2,-4,-3,-4,-2,-2 ];
var data4 = [ -6,-3,-2,-3,-3,-3,-2,-1,-2,-3,-1,-1,-1,-1,-1,-3,-3 ];
var data = [ labels, data1, data2, data3, data4 ];

var g = new Perl('Chart::LinesPoints', 600, 300);
var formatter = function(x) {
  // just a trick, to let the y scale start at the biggest point:
  // initiate with negative values, remove the minus sign!
  return String(x).substr(1, 2);
}
g.set(
  'integer_ticks_only', 'true',
  'title', "Soccer Season 2002",
  'legend_labels', [
    'NY Soccer Club',
    'Denver Tigers',
    'Houston Spacecats',
    'Washington Presidents'],
  'y_label', 'position in the table',
  'x_label', 'day of play',
  'grid_lines', 'true',
  'f_y_tick', formatter);
g.png('chart.png', data);
print("wrote chart.png");

