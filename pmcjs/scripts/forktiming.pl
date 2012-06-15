
my $i;
for ($i = 0; $i < 1000; ++$i) {
  my $pid = fork();
  if ($pid == 0) {
    # exec('perl', 'child.pl');
    exec('jspl', 'child.js');
    exit -1;
  }
  waitpid($pid, 0);
}

