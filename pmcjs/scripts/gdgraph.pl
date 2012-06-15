
use GD::Graph::bars;
my @data = (
           ["1st","2nd","3rd","4th","5th","6th","7th", "8th", "9th"],
           [    1,    2,    5,    6,    3,  1.5,    1,     3,     4]
#           [ sort { $a <=> $b } (1, 2, 5, 6, 3, 1.5, 1, 3, 4) ]
         );

my $graph = GD::Graph::bars->new(400, 300);
$graph->set(
  x_label           => 'X 軸',
  y_label           => 'Y 軸',
  title             => '単純なグラフ',
  y_max_value       => 8,
  y_tick_number     => 8,
  y_label_skip      => 2
) or die $graph->error;

my $gd = $graph->plot(\@data) or die $graph->error;

open(IMG, '>file.png') or die $!;
binmode IMG;
print IMG $gd->png;

