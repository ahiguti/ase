
package EvalTest;

sub evaltest {
  $r = JavaScript->Eval("10 * 30 * 4");
  print "r=$r\n";
  $r = JavaScript->Eval("foo()"); # throws
  print "r=$r\n";
};

1;
