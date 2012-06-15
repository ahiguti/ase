
# should not work

my $loop = 100;
my $numthreads = 3;
my $dump = 1;

my $Thread = $GCJ->{'java.lang.Thread'};
my $System = $GCJ->{'java.lang.System'};

sub WorkerThread::new() {
  my $x = {};
  $x->{id} = $_[1];
  return bless $x, $_[0];
}

sub WorkerThread::run() {
  my $id = $_[0];
  for (my $j = 0; $j < $loop; ++$j) {
    if ($dump) {
      print "id=$id j=$j\n";
    }
  }
}

my $arr = [];
for (my $i = 0; $i < $numthreads; ++$i) {
  $arr->[$i] = $Thread->NewInstance(new WorkerThread($i));
}
for (my $i = 0; $i < $numthreads; ++$i) {
  $arr->[$i]->start();
}
for (my $i = 0; $i < $numthreads; ++$i) {
  $arr->[$i]->join();
  print "id=$i join done\n";
}

