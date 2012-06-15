
package FuncObj;

sub testfunc {
  my $f = shift;
  return $f->(10, 20, 30);
};

1;

