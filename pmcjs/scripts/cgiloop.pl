
use CGI;
for (my $i = 0; $i < 100000; $i++) {
  $q = new CGI;
  my $s = print $q->start_html('hello');
}
0;
