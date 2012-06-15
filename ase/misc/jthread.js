
var loop = 1000000;
var numthreads = 10;
var dump = false;

var Thread = GCJ['java.lang.Thread'];
var System = GCJ['java.lang.System'];

function WorkerThread(id) {
  this.run = function() {
    for (var j = 0; j < loop; ++j) {
      if (dump) {
	print("id=" + id + " j=" + j);
      }
      if ((j & 4095) == 0) {
	System.gc();
        // Thread.yield();
	gc();
      }
    }
  }
}

var arr = new Array(numthreads);
for (var i = 0; i < numthreads; ++i) {
  arr[i] = Thread.NewInstance(new WorkerThread(i));
}
for (var i = 0; i < numthreads; ++i) {
  arr[i].start();
}
for (var i = 0; i < numthreads; ++i) {
  arr[i].join();
  print("id=" + i + " join done");
}

