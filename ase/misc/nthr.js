
var Thread = Native.Util.Thread;

function Worker(id) {
  return function() {
    for (var j = 1; j <= 10000; ++j) {
      print("thread " + id + " j=" + j);
      // Thread.noop();
      Thread.yield();
    }
  }
}

var num = 5;
var thr = new Array(num);
for (var i = 0; i < num; ++i) {
  thr[i] = Thread(Worker(i));
}
try {
  for (var i = 0; i < num; ++i) {
    thr[i].start();
  }
} finally {
  for (var i = 0; i < num; ++i) {
    thr[i].join();
    print("id=" + i + " join done");
  }
}

