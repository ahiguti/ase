
package JSEval;

sub jsevaltest {
  my $f = JavaScript->Eval('var x = function(x) { return x + x; }; x;');
  my $x = $f->(10);
  print $x, "\n";
}

1;
