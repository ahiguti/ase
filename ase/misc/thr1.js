
var Thread = GCJ['java.lang.Thread'];
var System = GCJ['java.lang.System'];

function WorkerThread(id) {
  this.run = function() {
    for (var j = 0; j < 100000; ++j) {
      // print("id=" + id + " j=" + j);
      Thread.yield();
      // System.gc();
    }
  }
}

var num = 10;
var arr = new Array(num);
for (var i = 0; i < num; ++i) {
  arr[i] = Thread.NewInstance(new WorkerThread(i));
}
try {
  for (var i = 0; i < num; ++i) {
    arr[i].start();
  }
} finally {
  for (var i = 0; i < num; ++i) {
    arr[i].join();
    print("id=" + i + " join done");
  }
}

