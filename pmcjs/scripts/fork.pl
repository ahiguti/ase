
my $pid = fork();
if ($pid == 0) {
  print("child\n");
  exit(3);
}
my $k = waitpid($pid, 0);
my $st = $? >> 8;
print("$st\n");
