
var Thread = Java['java.lang.Thread'];
var System = Java['java.lang.System'];
var Runnable = Java['java.lang.Runnable'];

function WorkerThread(id) {
  this.run = function() {
    for (var j = 1; j <= 10000; ++j) {
      print("thread " + id + " j=" + j);
      Thread.yield();
    }
  }
}

var num = 5;
var wkr = [];
var thr = new Array(num);
for (var i = 0; i < num; ++i) {
  wkr[i] = new WorkerThread(i);
  thr[i] = Thread(Runnable(wkr[i]));
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

